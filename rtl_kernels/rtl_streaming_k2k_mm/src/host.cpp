/**
* Copyright (C) 2019-2021 Xilinx, Inc
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
#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"
#include "xrt/xrt_bo.h"
#include "xrt/xrt_hw_context.h"

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

auto constexpr c_test_size = 1 * 1024 * 1024; // 1 MB data

////////MAIN FUNCTION//////////
int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }
    int size = c_test_size;

    const char* xcl_mode = std::getenv("XCL_EMULATION_MODE");
    if (xcl_mode != nullptr && std::string(xcl_mode) == "hw_emu") {
        size = 4096; // 4KB for HW emulation
    } else if (xcl_mode != nullptr) {
        size = 2 * 1024 * 1024; // 2MB for sw emulation
    }

    // I/O Data Vectors
    std::vector<int, aligned_allocator<int> > a(size);
    std::vector<int, aligned_allocator<int> > hw_results(size);
    std::vector<int> sw_results(size);

    std::generate(a.begin(), a.end(), std::rand);

    for (int i = 0; i < size; i++) {
        sw_results[i] = a[i] + 2;
    }

    // XRT Native API Host Code
    auto binaryFile = argv[1];

    xrt::device device(0);
    auto uuid = device.load_xclbin(binaryFile);
    xrt::hw_context hw_ctx(device, uuid);

    std::cout << "Device[0]: program successful!\n";

    xrt::kernel krnl_mm2s(hw_ctx, "krnl_mm2s");
    xrt::kernel krnl_myadder1(hw_ctx, "myadder1:{myadder1_1}");
    xrt::kernel krnl_myadder2(hw_ctx, "myadder2:{myadder2_1}");
    xrt::kernel krnl_s2mm(hw_ctx, "krnl_s2mm");

    std::cout << "Vector Increment of elements 0x" << std::hex << size << " by 2" << std::endl;

    // Running the kernel
    unsigned int vector_size_bytes = size * sizeof(int);

    // Allocate Buffers in Global Memory
    xrt::bo buffer_input(hw_ctx, vector_size_bytes, krnl_mm2s.group_id(0));
    xrt::bo buffer_output(hw_ctx, vector_size_bytes, krnl_s2mm.group_id(0));

    // Map and write input data
    auto map_input = buffer_input.map<int*>();
    for (int i = 0; i < size; i++) {
        map_input[i] = a[i];
    }
    buffer_input.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    // Launch the Kernels (streaming k2k with memory-mapped endpoints)
    // Note: arg index 1 is stream (not set via host), so use set_arg for indices 0 and 2
    xrt::run run_mm2s(krnl_mm2s);
    run_mm2s.set_arg(0, buffer_input);
    run_mm2s.set_arg(2, size);
    run_mm2s.start();

    xrt::run run_myadder1(krnl_myadder1);
    run_myadder1.start();

    xrt::run run_myadder2(krnl_myadder2);
    run_myadder2.start();

    xrt::run run_s2mm(krnl_s2mm);
    run_s2mm.set_arg(0, buffer_output);
    run_s2mm.set_arg(2, size);
    run_s2mm.start();

    run_mm2s.wait();
    run_myadder1.wait();
    run_myadder2.wait();
    run_s2mm.wait();

    // Copy Result from Device Global Memory to Host Local Memory
    buffer_output.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
    auto map_output = buffer_output.map<int*>();
    for (int i = 0; i < size; i++) {
        hw_results[i] = map_output[i];
    }

    // Compare the device results with software results
    bool match = std::equal(sw_results.begin(), sw_results.end(), hw_results.begin());
    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;

    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
