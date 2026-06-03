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
    size_t vector_size_bytes = sizeof(int) * size;
    std::vector<int, aligned_allocator<int> > source_input1(size);
    std::vector<int, aligned_allocator<int> > source_input2(size);
    std::vector<int, aligned_allocator<int> > source_input3(size);
    std::vector<int, aligned_allocator<int> > source_input4(size);
    std::vector<int, aligned_allocator<int> > source_krnl0_output(size);
    std::vector<int, aligned_allocator<int> > source_hw_results(size);
    std::vector<int, aligned_allocator<int> > source_sw_results(size);

    // Create the test data and Software Result
    for (int i = 0; i < size; i++) {
        source_input1[i] = i;
        source_input2[i] = i;
        source_input3[i] = i;
        source_input4[i] = i;
        source_sw_results[i] = source_input1[i] + source_input2[i] + source_input4[i];
        source_krnl0_output[i] = 0;
        source_hw_results[i] = 0;
    }

    // XRT Native API HOST CODE AREA START
    xrt::device device(0);
    auto uuid = device.load_xclbin(binaryFile);
    xrt::hw_context hw_ctx(device, uuid);

    std::cout << "Device[0]: program successful!\n";

    xrt::kernel krnl_vadd_0(hw_ctx, "krnl_vadd_rtl_0");
    xrt::kernel krnl_vadd_1(hw_ctx, "krnl_vadd_rtl_1");

    // Allocate Buffers in Global Memory
    xrt::bo buffer_r1(hw_ctx, vector_size_bytes, krnl_vadd_0.group_id(0));
    xrt::bo buffer_r2(hw_ctx, vector_size_bytes, krnl_vadd_0.group_id(1));
    xrt::bo buffer_rw_0(hw_ctx, vector_size_bytes, krnl_vadd_0.group_id(2));
    xrt::bo buffer_rw_1(hw_ctx, vector_size_bytes, krnl_vadd_1.group_id(0));
    xrt::bo buffer_r3(hw_ctx, vector_size_bytes, krnl_vadd_1.group_id(1));
    xrt::bo buffer_w(hw_ctx, vector_size_bytes, krnl_vadd_1.group_id(2));

    // Map and write input data
    auto map_r1 = buffer_r1.map<int*>();
    auto map_r2 = buffer_r2.map<int*>();
    auto map_r3 = buffer_r3.map<int*>();
    for (int i = 0; i < size; i++) {
        map_r1[i] = source_input1[i];
        map_r2[i] = source_input2[i];
        map_r3[i] = source_input4[i];
    }
    buffer_r1.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    buffer_r2.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    buffer_r3.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    // Launch Kernel 0
    auto run_0 = krnl_vadd_0(buffer_r1, buffer_r2, buffer_rw_0, size);
    run_0.wait();

    // Copy buffer_rw_0 to buffer_rw_1
    buffer_rw_1.copy(buffer_rw_0, vector_size_bytes);

    // Launch Kernel 1
    auto run_1 = krnl_vadd_1(buffer_rw_1, buffer_r3, buffer_w, size);
    run_1.wait();

    // Copy Result from Device Global Memory to Host Local Memory
    buffer_w.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
    auto map_w = buffer_w.map<int*>();
    buffer_rw_0.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
    auto map_rw_0 = buffer_rw_0.map<int*>();
    for (int i = 0; i < size; i++) {
        source_hw_results[i] = map_w[i];
        source_krnl0_output[i] = map_rw_0[i];
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
        std::cout << "i = " << i << " Software result = " << source_sw_results[i]
                  << " Device result = " << source_hw_results[i] << " input1 = " << source_input1[i]
                  << " input2 = " << source_input2[i] << " krnl0_output = " << source_krnl0_output[i]
                  << " input3 = " << source_input4[i] << std::endl;
    }

    std::cout << "TEST " << (match ? "FAILED" : "PASSED") << std::endl;
    return (match ? EXIT_FAILURE : EXIT_SUCCESS);
}
