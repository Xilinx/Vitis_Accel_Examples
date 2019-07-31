/**********
Copyright (c) 2018, Xilinx, Inc.
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
*******************************************************************/
#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

// This extension file is required for stream APIs
#include "CL/cl_ext_xilinx.h"
// This file is required for OpenCL C++ wrapper APIs
#include "xcl2.hpp"

auto constexpr c_test_size = 256 * 1024 * 1024; //256 MB data

////////////////////RESET FUNCTION//////////////////////////////////
int reset(int *a,
          int *b,
          int *c,
          int *sw_results,
          int *hw_results,
          unsigned int size) {
    //Fill the input vectors with data
    std::generate(a, a + size, std::rand);
    std::generate(b, b + size, std::rand);
    std::generate(c, c + size, std::rand);
    for (size_t i = 0; i < size; i++) {
        hw_results[i] = 0;
        sw_results[i] = (a[i] + b[i]) * c[i];
    }
    return 0;
}
///////////////////VERIFY FUNCTION///////////////////////////////////
bool verify(int *sw_results, int *hw_results, int size) {
    bool match = true;
    for (int i = 0; i < size; i++) {
        if (sw_results[i] != hw_results[i]) {
            match = false;
            break;
        }
    }
    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;
    return match;
}
////////MAIN FUNCTION//////////
int main(int argc, char **argv) {
    unsigned int size = c_test_size;

    if (xcl::is_hw_emulation()) {
        size = 4096; // 4KB for HW emulation
    } else if (xcl::is_emulation()) {
        size = 2 * 1024 * 1024; // 4MB for sw emulation
    }

    // I/O Data Vectors
    std::vector<int, aligned_allocator<int>> h_a(size);
    std::vector<int, aligned_allocator<int>> h_b(size);
    std::vector<int, aligned_allocator<int>> h_c(size);
    std::vector<int, aligned_allocator<int>> hw_results(size);
    std::vector<int> sw_results(size);

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    // OpenCL Host Code Begins.
    cl_int err;

    // OpenCL objects
    cl::Device device;
    cl::Context context;
    cl::CommandQueue q;
    cl::Program program;
    cl::Kernel krnl_vadd;
    cl::Kernel krnl_vmult;

    auto binaryFile = argv[1];

    // get_xil_devices() is a utility API which will find the xilinx
    // platforms and will return list of devices connected to Xilinx platform
    auto devices = xcl::get_xil_devices();

    // Selecting the first available Xilinx device
    device = devices[0];

    // Creating Context
    OCL_CHECK(err, context = cl::Context(device, NULL, NULL, NULL, &err));

    // Creating Command Queue
    OCL_CHECK(err,
              q = cl::CommandQueue(context,
                                   device,
                                   CL_QUEUE_PROFILING_ENABLE |
                                       CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE,
                                   &err));

    // read_binary_file() is a utility API which will load the binaryFile
    // and will return the pointer to file buffer.
   auto fileBuf = xcl::read_binary_file(binaryFile);

   cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    devices.resize(1);

    // Creating Program
    OCL_CHECK(err, program = cl::Program(context, devices, bins, NULL, &err));

    // Creating Kernel
    OCL_CHECK(err, krnl_vadd = cl::Kernel(program, "krnl_stream_vadd", &err));
    OCL_CHECK(err, krnl_vmult = cl::Kernel(program, "krnl_stream_vmult", &err));

    std::cout << "Vector Addition and Multiplication of elements 0x" << std::hex
              << size << std::endl;

    // Reset the data vectors
    reset(h_a.data(),
          h_b.data(),
          h_c.data(),
          sw_results.data(),
          hw_results.data(),
          size);

    //Running the kernel
    unsigned int vector_size_bytes = size * sizeof(int);

    // Allocate Buffer in Global Memory
    // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and
    // Device-to-host communication
    OCL_CHECK(err,
              cl::Buffer buffer_in1(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    vector_size_bytes,
                                    h_a.data(),
                                    &err));
    OCL_CHECK(err,
              cl::Buffer buffer_in2(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    vector_size_bytes,
                                    h_b.data(),
                                    &err));
    OCL_CHECK(err,
              cl::Buffer buffer_in3(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    vector_size_bytes,
                                    h_c.data(),
                                    &err));
    OCL_CHECK(err,
              cl::Buffer buffer_output(context,
                                       CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                                       vector_size_bytes,
                                       hw_results.data(),
                                       &err));

    // Setting Kernel Arguments
    OCL_CHECK(err, err = krnl_vadd.setArg(0, buffer_in1));
    OCL_CHECK(err, err = krnl_vadd.setArg(1, buffer_in2));
    OCL_CHECK(err, err = krnl_vadd.setArg(3, size));

    OCL_CHECK(err, err = krnl_vmult.setArg(0, buffer_in3));
    OCL_CHECK(err, err = krnl_vmult.setArg(2, buffer_output));
    OCL_CHECK(err, err = krnl_vmult.setArg(3, size));

    // Copy input data to device global memory
    OCL_CHECK(
        err,
        err = q.enqueueMigrateMemObjects({buffer_in1, buffer_in2, buffer_in3},
                                         0 /* 0 means from host*/));

    // Launch the Kernel
    OCL_CHECK(err, err = q.enqueueTask(krnl_vadd));
    OCL_CHECK(err, err = q.enqueueTask(krnl_vmult));
    q.finish();

    // Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err,
              err = q.enqueueMigrateMemObjects({buffer_output},
                                               CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();
    // OpenCL Host Code Ends

    // Compare the device results with software results
    bool match = verify(sw_results.data(), hw_results.data(), size);

    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
