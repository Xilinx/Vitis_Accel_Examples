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

/********************************************************************************************
 * Description:
 * This is host application to test HBM Full bandwidth.
 * Design contains 8 compute units of Kernel. Each compute unit has full access
 *to all HBM
 * memory (0 to 31). Host application allocate buffers into all 32 HBM Banks(16
 *Input buffers
 * and 16 output buffers). Host application runs all 8 compute units together
 *and measure
 * the overall HBM bandwidth.
 *
 ******************************************************************************************/

#include <algorithm>
#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <cstring>
#include <chrono>

#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"
#include "xrt/xrt_bo.h"
#include "xrt/xrt_hw_context.h"

#define NUM_KERNEL 3

// Aligned allocator for host memory (replacement for xcl2.hpp aligned_allocator)
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

// Function for verifying results
bool verify(std::vector<int, aligned_allocator<int> >& source_sw_add_results,
            std::vector<int, aligned_allocator<int> >& source_sw_mul_results,
            std::vector<int, aligned_allocator<int> >& source_hw_add_results,
            std::vector<int, aligned_allocator<int> >& source_hw_mul_results,
            unsigned int size) {
    bool check = true;
    for (size_t i = 0; i < size; i++) {
        if (source_hw_add_results[i] != source_sw_add_results[i]) {
            std::cout << "Error: Result mismatch in Addition Operation" << std::endl;
            std::cout << "i = " << i << " CPU result = " << source_sw_add_results[i]
                      << " Device result = " << source_hw_add_results[i] << std::endl;
            check = false;
            break;
        }
        if (source_hw_mul_results[i] != source_sw_mul_results[i]) {
            std::cout << "Error: Result mismatch in Multiplication Operation" << std::endl;
            std::cout << "i = " << i << " CPU result = " << source_sw_mul_results[i]
                      << " Device result = " << source_hw_mul_results[i] << std::endl;
            check = false;
            break;
        }
    }
    return check;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <XCLBIN> \n", argv[0]);
        return -1;
    }

    unsigned int dataSize = 64 * 1024 * 1024; // taking maximum possible data size value for an HBM bank
    unsigned int num_times = 1024;            // num_times specify, number of times a kernel
                                              // will execute the same operation. This is
                                              // needed
    // to keep the kernel busy to test the actual bandwidth of all banks running
    // concurrently.

    // reducing the test data capacity to run faster in emulation mode
    const char* xcl_mode = std::getenv("XCL_EMULATION_MODE");
    bool is_emulation = (xcl_mode != nullptr);
    if (is_emulation) {
        dataSize = 1024;
        num_times = 64;
    }

    std::string binaryFile = argv[1];
    std::string krnl_name = "krnl_vaddmul";
    std::vector<xrt::kernel> krnls(NUM_KERNEL);
    std::vector<int, aligned_allocator<int> > source_in1(dataSize);
    std::vector<int, aligned_allocator<int> > source_in2(dataSize);
    std::vector<int, aligned_allocator<int> > source_sw_add_results(dataSize);
    std::vector<int, aligned_allocator<int> > source_sw_mul_results(dataSize);

    std::vector<int, aligned_allocator<int> > source_hw_add_results[NUM_KERNEL];
    std::vector<int, aligned_allocator<int> > source_hw_mul_results[NUM_KERNEL];

    for (int i = 0; i < NUM_KERNEL; i++) {
        source_hw_add_results[i].resize(dataSize);
        source_hw_mul_results[i].resize(dataSize);
    }

    // Create the test data
    std::generate(source_in1.begin(), source_in1.end(), std::rand);
    std::generate(source_in2.begin(), source_in2.end(), std::rand);
    for (size_t i = 0; i < dataSize; i++) {
        source_sw_add_results[i] = source_in1[i] + source_in2[i];
        source_sw_mul_results[i] = source_in1[i] * source_in2[i];
    }

    // Initializing output vectors to zero
    for (size_t i = 0; i < NUM_KERNEL; i++) {
        std::fill(source_hw_add_results[i].begin(), source_hw_add_results[i].end(), 0);
        std::fill(source_hw_mul_results[i].begin(), source_hw_mul_results[i].end(), 0);
    }

    // XRT HOST CODE AREA START
    // Create XRT device and load xclbin
    xrt::device device(0);
    auto uuid = device.load_xclbin(binaryFile);
    xrt::hw_context hw_ctx(device, uuid);

    std::cout << "Device[0]: program successful!\n";

    // Creating Kernel objects using Compute unit names
    for (int i = 0; i < NUM_KERNEL; i++) {
        std::string cu_id = std::to_string(i + 1);
        std::string krnl_name_full = krnl_name + ":{" + "krnl_vaddmul_" + cu_id + "}";

        printf("Creating a kernel [%s] for CU(%d)\n", krnl_name_full.c_str(), i + 1);

        // Here Kernel object is created by specifying kernel name along with
        // compute unit.
        // For such case, this kernel object can only access the specific
        // Compute unit

        krnls[i] = xrt::kernel(hw_ctx, krnl_name_full);
    }

    std::vector<xrt::bo> buffer_input1(NUM_KERNEL);
    std::vector<xrt::bo> buffer_input2(NUM_KERNEL);
    std::vector<xrt::bo> buffer_output_add(NUM_KERNEL);
    std::vector<xrt::bo> buffer_output_mul(NUM_KERNEL);

    // Create XRT buffer objects - bank assignment is handled by kernel's connectivity
    // The group_id corresponds to the kernel argument index
    for (int i = 0; i < NUM_KERNEL; i++) {
        buffer_input1[i] = xrt::bo(hw_ctx, sizeof(uint32_t) * dataSize, krnls[i].group_id(0));
        buffer_input2[i] = xrt::bo(hw_ctx, sizeof(uint32_t) * dataSize, krnls[i].group_id(1));
        buffer_output_add[i] = xrt::bo(hw_ctx, sizeof(uint32_t) * dataSize, krnls[i].group_id(2));
        buffer_output_mul[i] = xrt::bo(hw_ctx, sizeof(uint32_t) * dataSize, krnls[i].group_id(3));

        // Write input data to buffers using map
        auto in1_map = buffer_input1[i].map<int*>();
        auto in2_map = buffer_input2[i].map<int*>();
        std::memcpy(in1_map, source_in1.data(), sizeof(uint32_t) * dataSize);
        std::memcpy(in2_map, source_in2.data(), sizeof(uint32_t) * dataSize);

        buffer_input1[i].sync(XCL_BO_SYNC_BO_TO_DEVICE);
        buffer_input2[i].sync(XCL_BO_SYNC_BO_TO_DEVICE);
    }

    double kernel_time_in_sec = 0, result = 0;

    std::chrono::duration<double> kernel_time(0);

    auto kernel_start = std::chrono::high_resolution_clock::now();
    std::vector<xrt::run> runs(NUM_KERNEL);
    for (int i = 0; i < NUM_KERNEL; i++) {
        // Execute kernel with inline argument passing
        runs[i] = krnls[i](buffer_input1[i], buffer_input2[i], buffer_output_add[i],
                           buffer_output_mul[i], dataSize, num_times);
    }
    // Wait for all kernels to complete
    for (int i = 0; i < NUM_KERNEL; i++) {
        runs[i].wait();
    }
    auto kernel_end = std::chrono::high_resolution_clock::now();

    kernel_time = std::chrono::duration<double>(kernel_end - kernel_start);

    kernel_time_in_sec = kernel_time.count();
    kernel_time_in_sec /= NUM_KERNEL;

    // Copy Result from Device Global Memory to Host Local Memory
    for (int i = 0; i < NUM_KERNEL; i++) {
        buffer_output_add[i].sync(XCL_BO_SYNC_BO_FROM_DEVICE);
        buffer_output_mul[i].sync(XCL_BO_SYNC_BO_FROM_DEVICE);

        auto out_add_map = buffer_output_add[i].map<int*>();
        auto out_mul_map = buffer_output_mul[i].map<int*>();
        std::memcpy(source_hw_add_results[i].data(), out_add_map, sizeof(uint32_t) * dataSize);
        std::memcpy(source_hw_mul_results[i].data(), out_mul_map, sizeof(uint32_t) * dataSize);
    }

    bool match = true;

    for (int i = 0; i < NUM_KERNEL; i++) {
        match = verify(source_sw_add_results, source_sw_mul_results, source_hw_add_results[i], source_hw_mul_results[i],
                       dataSize);
    }

    // Multiplying the actual data size by 4 because four buffers are being used.
    result = 4 * (float)dataSize * num_times * sizeof(uint32_t);
    result /= 1000;               // to KB
    result /= 1000;               // to MB
    result /= 1000;               // to GB
    result /= kernel_time_in_sec; // to GBps

    std::cout << "THROUGHPUT = " << result << " GB/s" << std::endl;
    // XRT HOST CODE AREA ENDS

    std::cout << (match ? "TEST PASSED" : "TEST FAILED") << std::endl;
    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
