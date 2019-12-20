/**********
Copyright (c) 2019, Xilinx, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/
#include "xcl2.hpp"
#include <algorithm>
#include <array>
#include <iostream>

#define MAT_DIM 8
#define MAT_SIZE MAT_DIM *MAT_DIM
#define NUM_TIMES 10
////////////////////UTILITY FUNCTION///////////////
void print_summary(
    std::string k1, std::string k2, double t1, double t2, int iterations);
////////////////////RESET FUNCTION/////////////////
int reset(int *a, int *b, int *c, int *d, int size) {
    //Fill the input vectors with data
    std::generate(a, a + size, std::rand);
    std::generate(b, b + size, std::rand);
    std::generate(c, c + size, std::rand);
    std::generate(d, d + size, std::rand);
    return 0;
}
///////////////////Software Results///////////////////////
void mmult_sw(std::vector<int, aligned_allocator<int>> &a,
              std::vector<int, aligned_allocator<int>> &b,
              std::vector<int, aligned_allocator<int>> &c,
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
int main(int argc, char **argv) {

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];
    int size = MAT_DIM * MAT_DIM;
    int vector_size_bytes = sizeof(int) * size;
    int err;
    cl::CommandQueue q;
    cl::Context context;
    cl::Kernel krnl_chain_mmult, krnl_simple_mmult;
    // Allocate Memory in Host Memory
    // When creating a buffer with user pointer (CL_MEM_USE_HOST_PTR), under the hood user ptr
    // is used if it is properly aligned. when not aligned, runtime had no choice but to create
    // its own host side buffer. So it is recommended to use this allocator if user wish to
    // create buffer using CL_MEM_USE_HOST_PTR to align user buffer to page boundary. It will
    // ensure that user buffer is used when user create Buffer/Mem object with CL_MEM_USE_HOST_PTR
    std::vector<std::vector<int, aligned_allocator<int>>> source_in1(NUM_TIMES);
    std::vector<std::vector<int, aligned_allocator<int>>> source_in2(NUM_TIMES);
    std::vector<std::vector<int, aligned_allocator<int>>> source_in3(NUM_TIMES);
    std::vector<std::vector<int, aligned_allocator<int>>> source_in4(NUM_TIMES);
    std::vector<std::vector<int, aligned_allocator<int>>> source_out12(
        NUM_TIMES);
    std::vector<std::vector<int, aligned_allocator<int>>> source_out123(
        NUM_TIMES);
    std::vector<std::vector<int, aligned_allocator<int>>> source_sw_results(
        NUM_TIMES);
    std::vector<std::vector<int, aligned_allocator<int>>> source_hw_results(
        NUM_TIMES);
    std::vector<std::vector<int, aligned_allocator<int>>> source_hw_results1(
        NUM_TIMES);

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

        reset(source_in1[i].data(),
              source_in2[i].data(),
              source_in3[i].data(),
              source_in4[i].data(),
              size);
        mmult_sw(source_in1[i], source_in2[i], source_out12[i], MAT_DIM);
        mmult_sw(source_out12[i], source_in3[i], source_out123[i], MAT_DIM);
        mmult_sw(
            source_out123[i], source_in4[i], source_sw_results[i], MAT_DIM);
    }

    // OPENCL HOST CODE AREA START
    // get_xil_devices() is a utility API which will find the xilinx
    // platforms and will return list of devices connected to Xilinx platform
    auto devices = xcl::get_xil_devices();

    // read_binary_file() is a utility API which will load the binaryFile
    // and will return the pointer to file buffer.
    auto fileBuf = xcl::read_binary_file(binaryFile);
    cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    int valid_device = 0;
    for (unsigned int i = 0; i < devices.size(); i++) {
        auto device = devices[i];
        // Creating Context and Command Queue for selected Device
        OCL_CHECK(err, context = cl::Context({device}, NULL, NULL, NULL, &err));
        OCL_CHECK(
            err,
            q = cl::CommandQueue(context,
                                 {device},
                                 CL_QUEUE_PROFILING_ENABLE |
                                     CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE,
                                 &err));

        std::cout << "Trying to program device[" << i
                  << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
        cl::Program program(context, {device}, bins, NULL, &err);
        if (err != CL_SUCCESS) {
            std::cout << "Failed to program device[" << i
                      << "] with xclbin file!\n";
        } else {
            std::cout << "Device[" << i << "]: program successful!\n";
            OCL_CHECK(err,
                      krnl_chain_mmult =
                          cl::Kernel(program, "krnl_chain_mmult", &err));
            OCL_CHECK(err,
                      krnl_simple_mmult =
                          cl::Kernel(program, "krnl_simple_mmult", &err));
            valid_device++;
            break; // we break because we found a valid device
        }
    }
    if (valid_device == 0) {
        std::cout << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }

    // Allocate Buffer in Global Memory
    // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and
    // Device-to-host communication
    cl::Buffer buffer_in1[NUM_TIMES], buffer_in2[NUM_TIMES],
        buffer_in3[NUM_TIMES], buffer_in4[NUM_TIMES], buffer_output[NUM_TIMES],
        buffer_output1[NUM_TIMES];
    for (int i = 0; i < NUM_TIMES; i++) {
        OCL_CHECK(err,
                  buffer_in1[i] =
                      cl::Buffer(context,
                                 CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                 vector_size_bytes,
                                 source_in1[i].data(),
                                 &err));
        OCL_CHECK(err,
                  buffer_in2[i] =
                      cl::Buffer(context,
                                 CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                 vector_size_bytes,
                                 source_in2[i].data(),
                                 &err));
        OCL_CHECK(err,
                  buffer_in3[i] =
                      cl::Buffer(context,
                                 CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                 vector_size_bytes,
                                 source_in3[i].data(),
                                 &err));
        OCL_CHECK(err,
                  buffer_in4[i] =
                      cl::Buffer(context,
                                 CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                 vector_size_bytes,
                                 source_in4[i].data(),
                                 &err));
        OCL_CHECK(err,
                  buffer_output[i] =
                      cl::Buffer(context,
                                 CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                                 vector_size_bytes,
                                 source_hw_results[i].data(),
                                 &err));

        OCL_CHECK(err,
                  buffer_output1[i] =
                      cl::Buffer(context,
                                 CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                                 vector_size_bytes,
                                 source_hw_results1[i].data(),
                                 &err));
    }

    //Kernel with ap_ctrl_chain
    auto start_chain = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_TIMES; i++) {
        OCL_CHECK(err, err = krnl_chain_mmult.setArg(0, buffer_in1[i]));
        OCL_CHECK(err, err = krnl_chain_mmult.setArg(1, buffer_in2[i]));
        OCL_CHECK(err, err = krnl_chain_mmult.setArg(2, buffer_in3[i]));
        OCL_CHECK(err, err = krnl_chain_mmult.setArg(3, buffer_in4[i]));
        OCL_CHECK(err, err = krnl_chain_mmult.setArg(4, buffer_output[i]));
        OCL_CHECK(err, err = krnl_chain_mmult.setArg(5, MAT_DIM));

        // Copy input data to device global memory
        OCL_CHECK(
            err,
            err = q.enqueueMigrateMemObjects(
                {buffer_in1[i], buffer_in2[i], buffer_in3[i], buffer_in4[i]},
                0 /* 0 means from host*/));

        // Launch the Kernel
        // For HLS kernels global and local size is always (1,1,1). So, it is recommended
        // to always use enqueueTask() for invoking HLS kernel
        OCL_CHECK(err, err = q.enqueueTask(krnl_chain_mmult));
    }

    OCL_CHECK(err, err = q.finish());

    auto end_chain = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < NUM_TIMES; i++) {
        // Copy Result from Device Global Memory to Host Local Memory
        OCL_CHECK(err,
                  err = q.enqueueMigrateMemObjects({buffer_output[i]},
                                                   CL_MIGRATE_MEM_OBJECT_HOST));
        OCL_CHECK(err, err = q.finish());
    }
    // OPENCL HOST CODE AREA END
    // Compare the results of the Device to the simulation
    bool match = true;
    for (int i = 0; i < NUM_TIMES; i++) {
        for (int j = 0; j < size; j++) {
            if (source_hw_results[i][j] != source_sw_results[i][j]) {
                std::cout << "Error: Result mismatch" << std::endl;
                std::cout << "i = " << i << " j = " << j
                          << " CPU result = " << source_sw_results[i][j]
                          << " Device result = " << source_hw_results[i][j]
                          << std::endl;
                match = false;
                break;
            }
        }
    }

    //Kernel without ap_ctrl_chain
    auto start_hs = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_TIMES; i++) {
        OCL_CHECK(err, err = krnl_simple_mmult.setArg(0, buffer_in1[i]));
        OCL_CHECK(err, err = krnl_simple_mmult.setArg(1, buffer_in2[i]));
        OCL_CHECK(err, err = krnl_simple_mmult.setArg(2, buffer_in3[i]));
        OCL_CHECK(err, err = krnl_simple_mmult.setArg(3, buffer_in4[i]));
        OCL_CHECK(err, err = krnl_simple_mmult.setArg(4, buffer_output1[i]));
        OCL_CHECK(err, err = krnl_simple_mmult.setArg(5, MAT_DIM));

        // Copy input data to device global memory
        OCL_CHECK(
            err,
            err = q.enqueueMigrateMemObjects(
                {buffer_in1[i], buffer_in2[i], buffer_in3[i], buffer_in4[i]},
                0 /* 0 means from host*/));

        // Launch the Kernel
        // For HLS kernels global and local size is always (1,1,1). So, it is recommended
        // to always use enqueueTask() for invoking HLS kernel
        OCL_CHECK(err, err = q.enqueueTask(krnl_simple_mmult));
    }

    OCL_CHECK(err, err = q.finish());
    auto end_hs = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < NUM_TIMES; i++) {
        // Copy Result from Device Global Memory to Host Local Memory
        OCL_CHECK(err,
                  err = q.enqueueMigrateMemObjects({buffer_output1[i]},
                                                   CL_MIGRATE_MEM_OBJECT_HOST));
        OCL_CHECK(err, err = q.finish());
    }
    // OPENCL HOST CODE AREA END
    // Compare the results of the Device to the simulation
    for (int i = 0; i < NUM_TIMES; i++) {
        for (int j = 0; j < size; j++) {
            if (source_hw_results1[i][j] != source_sw_results[i][j]) {
                std::cout << "Error: Result mismatch" << std::endl;
                std::cout << "i = " << i << " j = " << j
                          << " CPU result = " << source_sw_results[i][j]
                          << " Device result = " << source_hw_results1[i][j]
                          << std::endl;
                match &= false;
                break;
            }
        }
    }

    auto elapsed_chain =
        std::chrono::duration<double>(end_chain - start_chain).count();
    auto elapsed_hs = std::chrono::duration<double>(end_hs - start_hs).count();
    if (elapsed_chain < elapsed_hs) {
        print_summary("krnl_chain_mmult",
                      "krnl_simple_mmult",
                      elapsed_chain,
                      elapsed_hs,
                      NUM_TIMES);
    }
    bool test_status =
        xcl::is_emulation ? match : (match && (elapsed_chain < elapsed_hs));
    std::cout << "TEST " << (test_status ? "PASSED" : "FAILED") << std::endl;
    return (test_status ? EXIT_SUCCESS : EXIT_FAILURE);
}
void print_summary(
    std::string k1, std::string k2, double t1, double t2, int iterations) {
    double speedup = t2 / t1;
    printf("|-------------------------+-------------------------|\n"
           "| Kernel(%3d iterations)  |    Wall-Clock Time (s) |\n"
           "|-------------------------+-------------------------|\n",
           iterations);
    printf("| %-23s | %23f |\n", k1.c_str(), t1);
    printf("| %-23s | %23f |\n", k2.c_str(), t2);
    printf("|-------------------------+-------------------------|\n");
    printf("| Speedup: | %23lf |\n", speedup);
    printf("|-------------------------+-------------------------|\n");
    printf("Note: Wall Clock Time is meaningful for real hardware execution "
           "only, not for emulation.\n");
    printf("Please refer to profile summary for kernel execution time for "
           "hardware emulation.\n");
}
