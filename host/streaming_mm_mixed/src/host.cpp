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

// Declaration of custom stream APIs that binds to Xilinx Streaming APIs.
decltype(&clCreateStream) xcl::Stream::createStream = nullptr;
decltype(&clReleaseStream) xcl::Stream::releaseStream = nullptr;
decltype(&clReadStream) xcl::Stream::readStream = nullptr;
decltype(&clWriteStream) xcl::Stream::writeStream = nullptr;
decltype(&clPollStreams) xcl::Stream::pollStreams = nullptr;

auto constexpr c_test_size = 256 * 1024 * 1024; //256 MB data

////////////////////RESET FUNCTION//////////////////////////////////
int reset(int *a, int *b, int *sw_results, int *hw_results, unsigned int size) {
    //Fill the input vectors with data
    for (size_t i = 0; i < size; i++) {
        a[i] = rand() % size;
        b[i] = rand() % size;
        hw_results[i] = 0;
        sw_results[i] = a[i] + b[i];
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

    auto binaryFile = argv[1];
    unsigned fileBufSize;

    // get_xil_devices() is a utility API which will find the xilinx
    // platforms and will return list of devices connected to Xilinx platform
    auto devices = xcl::get_xil_devices();

    // Selecting the first available Xilinx device
    device = devices[0];
    cl_platform_id platform_id = device.getInfo<CL_DEVICE_PLATFORM>(&err);

    //Initialization of streaming class is needed before using it.
    xcl::Stream::init(platform_id);

    // Creating Context
    OCL_CHECK(err, context = cl::Context(device, NULL, NULL, NULL, &err));

    // Creating Command Queue
    OCL_CHECK(
        err,
        q = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err));

    // read_binary_file() is a utility API which will load the binaryFile
    // and will return the pointer to file buffer.
    auto fileBuf = xcl::read_binary_file(binaryFile, fileBufSize);

    cl::Program::Binaries bins{{fileBuf, fileBufSize}};
    devices.resize(1);

    // Creating Program
    OCL_CHECK(err, program = cl::Program(context, devices, bins, NULL, &err));

    // Creating Kernel
    OCL_CHECK(err, krnl_vadd = cl::Kernel(program, "krnl_stream_vadd", &err));

    std::cout << "Vector Addition of elements 0x" << std::hex << size
              << std::endl;

    // Reset the data vectors
    reset(h_a.data(), h_b.data(), sw_results.data(), hw_results.data(), size);

    //Running the kernel
    unsigned int vector_size_bytes = size * sizeof(int);

    // Allocate Buffer in Global Memory
    // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and
    // Device-to-host communication
    OCL_CHECK(err,
              cl::Buffer buffer_in2(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    vector_size_bytes,
                                    h_b.data(),
                                    &err));

    // Setting Kernel Arguments
    OCL_CHECK(err, err = krnl_vadd.setArg(2, buffer_in2));
    OCL_CHECK(err, err = krnl_vadd.setArg(3, size));

    // Copy input data to device global memory
    OCL_CHECK(err,
              err = q.enqueueMigrateMemObjects({buffer_in2},
                                               0 /* 0 means from host*/));

    // Launch the Kernel
    OCL_CHECK(err, err = q.enqueueTask(krnl_vadd));

    // Streams
    // Device Connection specification of the stream through extension pointer
    cl_int ret;
    cl_mem_ext_ptr_t ext;
    ext.param = krnl_vadd.get();
    ext.obj = NULL;

    //Create write stream for argument 0 and 1 of kernel
    cl_stream write_stream_a;
    ext.flags = 1;
    OCL_CHECK(ret,
              write_stream_a = xcl::Stream::createStream(
                  device.get(), CL_STREAM_WRITE_ONLY, CL_STREAM, &ext, &ret));

    //Create read stream for argument 2 of kernel
    cl_stream read_stream;
    ext.flags = 0;
    OCL_CHECK(ret,
              read_stream = xcl::Stream::createStream(
                  device.get(), CL_STREAM_READ_ONLY, CL_STREAM, &ext, &ret));

    // Initiating the WRITE transfer
    cl_stream_xfer_req wr_req{0};

    wr_req.flags = CL_STREAM_EOT;
    wr_req.priv_data = (void *)"write_a";

    // Thread 1 for writing data to input stream 1 independently in case of default blocking transfers.
    std::thread thr1(xcl::Stream::writeStream,
                     write_stream_a,
                     h_a.data(),
                     vector_size_bytes,
                     &wr_req,
                     &ret);

    // Initiating the READ transfer
    cl_stream_xfer_req rd_req{0};
    rd_req.flags = CL_STREAM_EOT;
    rd_req.priv_data = (void *)"read";
    // Output thread to read the stream data independently in case of default blocking transfers.
    std::thread thr2(xcl::Stream::readStream,
                     read_stream,
                     hw_results.data(),
                     vector_size_bytes,
                     &rd_req,
                     &ret);

    // Waiting for all the threads to complete their respective operations.
    thr1.join();
    thr2.join();

    // Releasing all OpenCL objects
    q.finish();
    xcl::Stream::releaseStream(read_stream);
    xcl::Stream::releaseStream(write_stream_a);
    // OpenCL Host Code Ends

    // Compare the device results with software results
    bool match = verify(sw_results.data(), hw_results.data(), size);

    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
