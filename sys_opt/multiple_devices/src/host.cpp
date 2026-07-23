/**
* Copyright (C) 2019-2021 Xilinx, Inc
* Copyright 2022-2026 Advanced Micro Devices, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License"). You may
* not use this file except in compliance with the License. A copy of the
* License is located at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
* License for the specific language governing permissions and limitations
* under the License.
*/

#include <array>
#include <iostream>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>
#include <chrono>
#include <cstdlib>
#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"
#include "xrt/xrt_bo.h"
#include "xrt/xrt_hw_context.h"
#include "xrt/experimental/xrt_system.h"

template <typename T>
struct aligned_allocator {
    using value_type = T;
    T* allocate(std::size_t num) {
        void* ptr = nullptr;
        if (posix_memalign(&ptr, 4096, num * sizeof(T))) throw std::bad_alloc();
        return reinterpret_cast<T*>(ptr);
    }
    void deallocate(T* p, std::size_t num) { free(p); }
};

std::string convert_size(size_t size) {
    const char* units[] = {"B", "KB", "MB", "GB"};
    int unit_idx = 0;
    double dsize = static_cast<double>(size);
    while (dsize >= 1024.0 && unit_idx < 3) {
        dsize /= 1024.0;
        unit_idx++;
    }
    char buf[64];
    snprintf(buf, sizeof(buf), "%.2f %s", dsize, units[unit_idx]);
    return std::string(buf);
}

using std::array;
using std::map;
using std::vector;

// This example demonstrates how to split work among multiple devices.
int main(int argc, char** argv) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File 1>"
                  << " <XCLBIN File 2>" << std::endl;
        return EXIT_FAILURE;
    }

    auto binaryFile1 = argv[1];
    auto binaryFile2 = argv[2];

    // Query the number of devices recognized by XRT. This returns the correct
    // count in both hardware and hardware emulation (matching emconfigutil --nd).
    unsigned int device_count = xrt::system::enumerate_devices();

    if (device_count == 0) {
        std::cout << "No devices found!" << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "Found " << device_count << " device(s)" << std::endl;

    const char* xcl_mode = std::getenv("XCL_EMULATION_MODE");
    bool is_hw_emu = (xcl_mode != nullptr && std::string(xcl_mode) == "hw_emu");

    static const int elements_per_device = is_hw_emu ? (1 << 10) : (1 << 20);
    static const int elements = elements_per_device * device_count;

    vector<int, aligned_allocator<int> > A(elements, 32);
    vector<int, aligned_allocator<int> > B(elements, 10);
    vector<int, aligned_allocator<int> > C(elements);

    // One element per device
    vector<xrt::device> devices(device_count);
    vector<xrt::uuid> uuids(device_count);
    vector<xrt::hw_context> contexts(device_count);
    vector<xrt::kernel> kernels(device_count);

    vector<xrt::bo> buffer_a(device_count);
    vector<xrt::bo> buffer_b(device_count);
    vector<xrt::bo> buffer_result(device_count);

    size_t size_per_device = elements_per_device * sizeof(int);
    static const int iter = is_hw_emu ? 2 : 10 * 1024;
    size_t total_size = iter * size_per_device * device_count * 3;
    std::string size_str = convert_size(total_size);

    std::cout << "Initializing XRT objects" << std::endl;
    for (int d = 0; d < (int)device_count; d++) {
        std::cout << "Creating Context[" << d << "]..." << std::endl;
        devices[d] = xrt::device(d);
        uuids[d] = devices[d].load_xclbin((d == 0) ? binaryFile1 : binaryFile2);
        contexts[d] = xrt::hw_context(devices[d], uuids[d]);
        kernels[d] = xrt::kernel(contexts[d], "vadd");

        // Allocate Buffers in Global Memory
        size_t offset = d * elements_per_device;
        std::cout << "Creating Buffers[" << d << "]..." << std::endl;
        buffer_a[d] = xrt::bo(contexts[d], size_per_device, kernels[d].group_id(1));
        buffer_b[d] = xrt::bo(contexts[d], size_per_device, kernels[d].group_id(2));
        buffer_result[d] = xrt::bo(contexts[d], size_per_device, kernels[d].group_id(0));

        // Map and write input data
        auto map_a = buffer_a[d].map<int*>();
        auto map_b = buffer_b[d].map<int*>();
        for (int i = 0; i < elements_per_device; i++) {
            map_a[i] = A[offset + i];
            map_b[i] = B[offset + i];
        }
    }

    std::chrono::high_resolution_clock::time_point TimeStart = std::chrono::high_resolution_clock::now();

    vector<xrt::run> runs(device_count);
    for (int d = 0; d < (int)device_count; d++) {
        // Copy input data to device global memory
        std::cout << "Copying data..." << std::endl;
        buffer_a[d].sync(XCL_BO_SYNC_BO_TO_DEVICE);
        buffer_b[d].sync(XCL_BO_SYNC_BO_TO_DEVICE);

        // Launch the Kernel
        std::cout << "Launching Kernel..." << std::endl;
        runs[d] = kernels[d](buffer_result[d], buffer_a[d], buffer_b[d], elements_per_device, iter);
    }

    for (int d = 0; d < (int)device_count; d++) {
        std::cout << "Waiting for work to finish on device " << d << std::endl;
        runs[d].wait();

        // Copy Result from Device Global Memory to Host Local Memory
        std::cout << "Getting Results..." << std::endl;
        buffer_result[d].sync(XCL_BO_SYNC_BO_FROM_DEVICE);
        auto map_result = buffer_result[d].map<int*>();
        size_t offset = d * elements_per_device;
        for (int i = 0; i < elements_per_device; i++) {
            C[offset + i] = map_result[i];
        }
    }

    std::chrono::high_resolution_clock::time_point TimeEnd = std::chrono::high_resolution_clock::now();
    double duration_in_ms = std::chrono::duration_cast<std::chrono::microseconds>(TimeEnd - TimeStart).count();

    // XRT HOST CODE AREA ENDS
    bool match = true;
    for (int i = 0; i < elements; i++) {
        int expected = A[i] + B[i];
        if (C[i] != expected) {
            std::cout << "Error: Result mismatch" << std::endl;
            std::cout << "i = " << i << " CPU result = " << expected << " Device result = " << C[i] << std::endl;
            match = false;
            break;
        }
    }

    double usduration = duration_in_ms;
    double dnsduration = duration_in_ms * 1000.0;
    double dsduration = duration_in_ms / 1000000.0;
    double bpersec = (total_size / dsduration);
    double mbpersec = bpersec / ((double)1024 * 1024);

    std::cout << "THROUGHPUT = " << mbpersec << " MB/s" << std::endl;
    std::cout << "Total Data = " << size_str << " Concurrency = " << device_count
              << " Duration = " << dnsduration << " ns" << std::endl;

    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;
    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
