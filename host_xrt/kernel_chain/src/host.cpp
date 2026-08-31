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
#include <algorithm>
#include <array>
#include <iostream>
#include <vector>
#include <chrono>
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

#define MAT_DIM 32
#define MAT_SIZE MAT_DIM* MAT_DIM
#define NUM_TIMES 10
////////////////////UTILITY FUNCTION///////////////
void print_summary(std::string k1, std::string k2, double t1, double t2, int iterations) {
    double speedup = t2 / t1;
    std::cout << "|-------------------------+-------------------------|\n"
              << "| Kernel(" << iterations << " iterations)  |    Wall-Clock Time (s)  |\n"
              << "|-------------------------+-------------------------|\n";
    std::cout << "| " << k1.c_str() << "        | " << t1 << "\t|\n";
    std::cout << "| " << k2.c_str() << "       | " << t2 << "\t|\n";
    std::cout << "|-------------------------+-------------------------|\n";
    std::cout << "| Speedup:                | " << speedup << "\t|\n";
    std::cout << "|-------------------------+-------------------------|\n";
    std::cout << "Note: Wall Clock Time is meaningful for real hardware "
                 "execution only, not for emulation.\n";
    std::cout << "Please refer to profile summary for kernel execution time for "
                 "hardware emulation.\n";
}

////////////////////RESET FUNCTION/////////////////
int reset(int* a, int* b, int* c, int* d, int size) {
    // Fill the input vectors with data
    std::generate(a, a + size, std::rand);
    std::generate(b, b + size, std::rand);
    std::generate(c, c + size, std::rand);
    std::generate(d, d + size, std::rand);
    return 0;
}
///////////////////Software Results///////////////////////
void mmult_sw(std::vector<int, aligned_allocator<int> >& a,
              std::vector<int, aligned_allocator<int> >& b,
              std::vector<int, aligned_allocator<int> >& c,
              int size) {
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            int result = 0;
            for (int k = 0; k < size; k++) {
                result += a[row * size + k] * b[k * size + col];
            }
            c[row * size + col] = result;
        }
    }
}
//////Main Function//////////////
int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];
    int size = MAT_DIM * MAT_DIM;
    size_t vector_size_bytes = sizeof(int) * size;
    // Allocate Memory in Host Memory
    // When creating a buffer with user pointer (CL_MEM_USE_HOST_PTR), under the
    // hood user ptr
    // is used if it is properly aligned. when not aligned, runtime had no choice
    // but to create
    // its own host side buffer. So it is recommended to use this allocator if
    // user wish to
    // create buffer using CL_MEM_USE_HOST_PTR to align user buffer to page
    // boundary. It will
    // ensure that user buffer is used when user create Buffer/Mem object with
    // CL_MEM_USE_HOST_PTR
    std::vector<std::vector<int, aligned_allocator<int> > > source_in1(NUM_TIMES);
    std::vector<std::vector<int, aligned_allocator<int> > > source_in2(NUM_TIMES);
    std::vector<std::vector<int, aligned_allocator<int> > > source_in3(NUM_TIMES);
    std::vector<std::vector<int, aligned_allocator<int> > > source_in4(NUM_TIMES);
    std::vector<std::vector<int, aligned_allocator<int> > > source_out12(NUM_TIMES);
    std::vector<std::vector<int, aligned_allocator<int> > > source_out123(NUM_TIMES);
    std::vector<std::vector<int, aligned_allocator<int> > > source_sw_results(NUM_TIMES);
    std::vector<std::vector<int, aligned_allocator<int> > > source_hw_results(NUM_TIMES);
    std::vector<std::vector<int, aligned_allocator<int> > > source_hw_results1(NUM_TIMES);

    for (int i = 0; i < NUM_TIMES; i++) {
        source_in1[i].resize(size);
        source_in2[i].resize(size);
        source_in3[i].resize(size);
        source_in4[i].resize(size);
        source_out12[i].resize(size);
        source_out123[i].resize(size);
        source_sw_results[i].resize(size);
        source_hw_results[i].resize(size);
        source_hw_results1[i].resize(size);

        reset(source_in1[i].data(), source_in2[i].data(), source_in3[i].data(), source_in4[i].data(), size);
        mmult_sw(source_in1[i], source_in2[i], source_out12[i], MAT_DIM);
        mmult_sw(source_out12[i], source_in3[i], source_out123[i], MAT_DIM);
        mmult_sw(source_out123[i], source_in4[i], source_sw_results[i], MAT_DIM);
    }

    // XRT HOST CODE AREA START
    auto device = xrt::device(0);
    auto uuid = device.load_xclbin(binaryFile);
    xrt::hw_context hw_ctx(device, uuid);
    std::cout << "Device[0]: program successful!\n";

    auto krnl_chain_mmult = xrt::kernel(hw_ctx, "krnl_chain_mmult");
    auto krnl_simple_mmult = xrt::kernel(hw_ctx, "krnl_simple_mmult");

    // Allocate Buffers in Global Memory
    std::vector<xrt::bo> buffer_in1(NUM_TIMES), buffer_in2(NUM_TIMES), buffer_in3(NUM_TIMES), buffer_in4(NUM_TIMES),
        buffer_in5(NUM_TIMES), buffer_in6(NUM_TIMES), buffer_in7(NUM_TIMES), buffer_in8(NUM_TIMES),
        buffer_output(NUM_TIMES), buffer_output1(NUM_TIMES);
    for (int i = 0; i < NUM_TIMES; i++) {
        buffer_in1[i] = xrt::bo(hw_ctx, vector_size_bytes, krnl_chain_mmult.group_id(0));
        buffer_in2[i] = xrt::bo(hw_ctx, vector_size_bytes, krnl_chain_mmult.group_id(1));
        buffer_in3[i] = xrt::bo(hw_ctx, vector_size_bytes, krnl_chain_mmult.group_id(2));
        buffer_in4[i] = xrt::bo(hw_ctx, vector_size_bytes, krnl_chain_mmult.group_id(3));
        buffer_in5[i] = xrt::bo(hw_ctx, vector_size_bytes, krnl_simple_mmult.group_id(0));
        buffer_in6[i] = xrt::bo(hw_ctx, vector_size_bytes, krnl_simple_mmult.group_id(1));
        buffer_in7[i] = xrt::bo(hw_ctx, vector_size_bytes, krnl_simple_mmult.group_id(2));
        buffer_in8[i] = xrt::bo(hw_ctx, vector_size_bytes, krnl_simple_mmult.group_id(3));
        buffer_output[i] = xrt::bo(hw_ctx, vector_size_bytes, krnl_chain_mmult.group_id(4));
        buffer_output1[i] = xrt::bo(hw_ctx, vector_size_bytes, krnl_simple_mmult.group_id(4));

        // Populate input buffers from host data
        std::copy(source_in1[i].begin(), source_in1[i].end(), buffer_in1[i].map<int*>());
        std::copy(source_in2[i].begin(), source_in2[i].end(), buffer_in2[i].map<int*>());
        std::copy(source_in3[i].begin(), source_in3[i].end(), buffer_in3[i].map<int*>());
        std::copy(source_in4[i].begin(), source_in4[i].end(), buffer_in4[i].map<int*>());
        std::copy(source_in1[i].begin(), source_in1[i].end(), buffer_in5[i].map<int*>());
        std::copy(source_in2[i].begin(), source_in2[i].end(), buffer_in6[i].map<int*>());
        std::copy(source_in3[i].begin(), source_in3[i].end(), buffer_in7[i].map<int*>());
        std::copy(source_in4[i].begin(), source_in4[i].end(), buffer_in8[i].map<int*>());
    }

    // Kernel with ap_ctrl_chain
    std::vector<xrt::run> runs_chain(NUM_TIMES);
    auto start_chain = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_TIMES; i++) {
        // Copy input data to device global memory
        buffer_in1[i].sync(XCL_BO_SYNC_BO_TO_DEVICE);
        buffer_in2[i].sync(XCL_BO_SYNC_BO_TO_DEVICE);
        buffer_in3[i].sync(XCL_BO_SYNC_BO_TO_DEVICE);
        buffer_in4[i].sync(XCL_BO_SYNC_BO_TO_DEVICE);
        // Launch the Kernel
        runs_chain[i] =
            krnl_chain_mmult(buffer_in1[i], buffer_in2[i], buffer_in3[i], buffer_in4[i], buffer_output[i], MAT_DIM);
    }

    for (int i = 0; i < NUM_TIMES; i++) {
        runs_chain[i].wait();
    }

    auto end_chain = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < NUM_TIMES; i++) {
        // Copy Result from Device Global Memory to Host Local Memory
        buffer_output[i].sync(XCL_BO_SYNC_BO_FROM_DEVICE);
        std::copy(buffer_output[i].map<int*>(), buffer_output[i].map<int*>() + size, source_hw_results[i].begin());
    }
    // XRT HOST CODE AREA END
    // Compare the results of the Device to the simulation
    bool match = true;
    for (int i = 0; i < NUM_TIMES; i++) {
        for (int j = 0; j < size; j++) {
            if (source_hw_results[i][j] != source_sw_results[i][j]) {
                std::cout << "Error: Result mismatch" << std::endl;
                std::cout << "i = " << i << " j = " << j << " CPU result = " << source_sw_results[i][j]
                          << " Device result = " << source_hw_results[i][j] << std::endl;
                match = false;
                break;
            }
        }
    }

    // Kernel without ap_ctrl_chain
    std::vector<xrt::run> runs_hs(NUM_TIMES);
    auto start_hs = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_TIMES; i++) {
        // Copy input data to device global memory
        buffer_in5[i].sync(XCL_BO_SYNC_BO_TO_DEVICE);
        buffer_in6[i].sync(XCL_BO_SYNC_BO_TO_DEVICE);
        buffer_in7[i].sync(XCL_BO_SYNC_BO_TO_DEVICE);
        buffer_in8[i].sync(XCL_BO_SYNC_BO_TO_DEVICE);
        // Launch the Kernel
        runs_hs[i] =
            krnl_simple_mmult(buffer_in5[i], buffer_in6[i], buffer_in7[i], buffer_in8[i], buffer_output1[i], MAT_DIM);
    }

    for (int i = 0; i < NUM_TIMES; i++) {
        runs_hs[i].wait();
    }
    auto end_hs = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < NUM_TIMES; i++) {
        // Copy Result from Device Global Memory to Host Local Memory
        buffer_output1[i].sync(XCL_BO_SYNC_BO_FROM_DEVICE);
        std::copy(buffer_output1[i].map<int*>(), buffer_output1[i].map<int*>() + size, source_hw_results1[i].begin());
    }
    // XRT HOST CODE AREA END
    // Compare the results of the Device to the simulation
    for (int i = 0; i < NUM_TIMES; i++) {
        for (int j = 0; j < size; j++) {
            if (source_hw_results1[i][j] != source_sw_results[i][j]) {
                std::cout << "Error: Result mismatch" << std::endl;
                std::cout << "i = " << i << " j = " << j << " CPU result = " << source_sw_results[i][j]
                          << " Device result = " << source_hw_results1[i][j] << std::endl;
                match &= false;
                break;
            }
        }
    }

    auto elapsed_chain = std::chrono::duration<double>(end_chain - start_chain).count();
    auto elapsed_hs = std::chrono::duration<double>(end_hs - start_hs).count();
    print_summary("krnl_chain_mmult", "krnl_simple_mmult", elapsed_chain, elapsed_hs, NUM_TIMES);

    bool test_status = match;
    std::cout << "TEST " << (test_status ? "PASSED" : "FAILED") << std::endl;
    return (test_status ? EXIT_SUCCESS : EXIT_FAILURE);
}
