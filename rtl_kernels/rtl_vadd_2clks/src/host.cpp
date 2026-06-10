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
#include <iostream>
#include <vector>
#include <cstdlib>
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

#define DATA_SIZE 256

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];

    auto size = DATA_SIZE;
    // Allocate Memory in Host Memory
    auto vector_size_bytes = sizeof(int) * size;
    std::vector<int, aligned_allocator<int> > source_input1(size);
    std::vector<int, aligned_allocator<int> > source_input2(size);
    std::vector<int, aligned_allocator<int> > source_hw_results(size);
    std::vector<int, aligned_allocator<int> > source_sw_results(size);

    // Create the test data and Software Result
    for (int i = 0; i < size; i++) {
        source_input1[i] = i;
        source_input2[i] = i;
        source_sw_results[i] = source_input1[i] + source_input2[i];
        source_hw_results[i] = 0;
    }

    // XRT Native API HOST CODE AREA START
    xrt::device device(0);
    auto uuid = device.load_xclbin(binaryFile);
    xrt::hw_context hw_ctx(device, uuid);

    std::cout << "Device[0]: program successful!\n";

    xrt::kernel krnl_vadd(hw_ctx, "krnl_vadd_2clk_rtl");

    // Allocate Buffers in Global Memory
    xrt::bo buffer_r1(hw_ctx, vector_size_bytes, krnl_vadd.group_id(0));
    xrt::bo buffer_r2(hw_ctx, vector_size_bytes, krnl_vadd.group_id(1));
    xrt::bo buffer_w(hw_ctx, vector_size_bytes, krnl_vadd.group_id(2));

    // Map and write input data
    auto map_r1 = buffer_r1.map<int*>();
    auto map_r2 = buffer_r2.map<int*>();
    for (int i = 0; i < size; i++) {
        map_r1[i] = source_input1[i];
        map_r2[i] = source_input2[i];
    }
    buffer_r1.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    buffer_r2.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    // Launch the Kernel
    auto run = krnl_vadd(buffer_r1, buffer_r2, buffer_w, size);
    run.wait();

    // Copy Result from Device Global Memory to Host Local Memory
    buffer_w.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
    auto map_w = buffer_w.map<int*>();
    for (int i = 0; i < size; i++) {
        source_hw_results[i] = map_w[i];
    }

    // XRT HOST CODE AREA END

    // Compare the results of the Device to the simulation
    int match = 0;
    for (int i = 0; i < size; i++) {
        if (source_hw_results[i] != source_sw_results[i]) {
            std::cout << "Error: Result mismatch" << std::endl;
            std::cout << "i = " << i << " Software result = " << source_sw_results[i]
                      << " Device result = " << source_hw_results[i] << std::endl;
            match = 1;
            break;
        }
    }

    std::cout << "TEST " << (match ? "FAILED" : "PASSED") << std::endl;
    return (match ? EXIT_FAILURE : EXIT_SUCCESS);
}
