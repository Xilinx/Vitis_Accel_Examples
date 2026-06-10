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

/*******************************************************************************
Description: Vitis Vector Addition using Streams
*******************************************************************************/

#define DATA_SIZE 4096
#define INCR_VALUE 10

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

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];

    // Allocate Memory in Host Memory
    auto vector_size_bytes = sizeof(int) * DATA_SIZE;

    std::vector<int, aligned_allocator<int> > source_input(DATA_SIZE);
    std::vector<int, aligned_allocator<int> > source_hw_results(DATA_SIZE);
    std::vector<int, aligned_allocator<int> > source_sw_results(DATA_SIZE);

    // Create the test data and Software Result
    for (int i = 0; i < DATA_SIZE; i++) {
        source_input[i] = i;
        source_sw_results[i] = i + INCR_VALUE;
        source_hw_results[i] = 0;
    }

    // XRT Native API HOST CODE AREA START
    xrt::device device(0);
    auto uuid = device.load_xclbin(binaryFile);
    xrt::hw_context hw_ctx(device, uuid);

    std::cout << "Device[0]: program successful!\n";

    xrt::kernel krnl_adder_stage(hw_ctx, "krnl_adder_stage_rtl");
    xrt::kernel krnl_input_stage(hw_ctx, "krnl_input_stage_rtl");
    xrt::kernel krnl_output_stage(hw_ctx, "krnl_output_stage_rtl");

    // Allocate Buffer in Global Memory
    xrt::bo buffer_input(hw_ctx, vector_size_bytes, krnl_input_stage.group_id(0));
    xrt::bo buffer_output(hw_ctx, vector_size_bytes, krnl_output_stage.group_id(0));

    // Map and write input data
    auto map_input = buffer_input.map<int*>();
    for (int i = 0; i < DATA_SIZE; i++) {
        map_input[i] = source_input[i];
    }
    buffer_input.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    auto inc = INCR_VALUE;
    auto size = DATA_SIZE;

    // Launch the Kernels
    auto run_input = krnl_input_stage(buffer_input, size);
    auto run_adder = krnl_adder_stage(inc, size);
    auto run_output = krnl_output_stage(buffer_output, size);

    // Wait for all kernels to finish
    run_input.wait();
    run_adder.wait();
    run_output.wait();

    // Copy Result from Device Global Memory to Host Local Memory
    buffer_output.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
    auto map_output = buffer_output.map<int*>();
    for (int i = 0; i < DATA_SIZE; i++) {
        source_hw_results[i] = map_output[i];
    }

    // XRT HOST CODE AREA END

    // Compare the results of the Device to the simulation
    int match = 0;
    for (int i = 0; i < DATA_SIZE; i++) {
        if (source_hw_results[i] != source_sw_results[i]) {
            std::cout << "Error: Result mismatch" << std::endl;
            std::cout << "i = " << i << " CPU result = " << source_sw_results[i]
                      << " Device result = " << source_hw_results[i] << std::endl;
            match = 1;
            break;
        }
    }

    std::cout << "TEST " << (match ? "FAILED" : "PASSED") << std::endl;
    return (match ? EXIT_FAILURE : EXIT_SUCCESS);
}
