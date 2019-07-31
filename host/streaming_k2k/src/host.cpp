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
int verify(int *sw_results, int *hw_results, int size) {
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
    size_t size = c_test_size;
    if (xcl::is_hw_emulation()) {
        size = 4096; // 4KB for HW emulation
    } else if (xcl::is_emulation()) {
        size = 2 * 1024 * 1024; // 2MB for sw emulation
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

    auto binaryFile = argv[1];
    std::cout << "Vector Addition and Multiplication of elements 0x" << std::hex
              << size << std::endl;

    // Reset the data vectors
    reset(h_a.data(),
          h_b.data(),
          h_c.data(),
          sw_results.data(),
          hw_results.data(),
          size);

    // OpenCL Setup
    // OpenCL objects
    cl::Device device;
    cl::Context context;
    cl::CommandQueue q;
    cl::Program program;
    cl::Kernel krnl_vadd;
    cl::Kernel krnl_vmult;

    // Error Status variable
    cl_int err;

    // get_xil_devices() is a utility API which will find the xilinx
    // platforms and will return list of devices connected to Xilinx platform
    auto devices = xcl::get_xil_devices();

    // Selecting the first available Xilinx device
    device = devices[0];
    auto platform_id = device.getInfo<CL_DEVICE_PLATFORM>(&err);

    //Initialization of streaming class is needed before using it.
    xcl::Stream::init(platform_id);

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

    size_t vector_size_bytes = size * sizeof(int);

    // Streams
    // Device Connection specification of the stream through extension pointer
    cl_int ret;
    cl_mem_ext_ptr_t ext1, ext2;
    ext1.param = krnl_vadd.get();
    ext2.param = krnl_vmult.get();
    ext1.obj = NULL;
    ext2.obj = NULL;

    //Create write stream for argument 0 and 1 of kernel
    cl_stream write_stream_a, write_stream_b, write_stream_c;
    ext1.flags = 0;
    OCL_CHECK(ret,
              write_stream_a = xcl::Stream::createStream(
                  device.get(), CL_STREAM_WRITE_ONLY, CL_STREAM, &ext1, &ret));
    ext1.flags = 1;
    OCL_CHECK(ret,
              write_stream_b = xcl::Stream::createStream(
                  device.get(), CL_STREAM_WRITE_ONLY, CL_STREAM, &ext1, &ret));
    ext2.flags = 0;
    OCL_CHECK(ret,
              write_stream_c = xcl::Stream::createStream(
                  device.get(), CL_STREAM_WRITE_ONLY, CL_STREAM, &ext2, &ret));

    //Create read stream for argument 2 of kernel
    cl_stream read_stream;
    ext2.flags = 2;
    OCL_CHECK(ret,
              read_stream = xcl::Stream::createStream(
                  device.get(), CL_STREAM_READ_ONLY, CL_STREAM, &ext2, &ret));

    OCL_CHECK(err, err = q.enqueueTask(krnl_vadd));
    OCL_CHECK(err, err = q.enqueueTask(krnl_vmult));

    // Initiating the WRITE transfer
    cl_stream_xfer_req wr_req{0};

    wr_req.flags = CL_STREAM_EOT | CL_STREAM_NONBLOCKING;
    wr_req.priv_data = (void *)"write_a";

    // Writing data to input stream 1 independently in case of non-blocking transfers.
    OCL_CHECK(
        ret,
        xcl::Stream::writeStream(
            write_stream_a, h_a.data(), vector_size_bytes, &wr_req, &ret));

    wr_req.priv_data = (void *)"write_b";
    // Writing data to input stream 2 independently in case of non-blocking transfers.
    OCL_CHECK(
        ret,
        xcl::Stream::writeStream(
            write_stream_b, h_b.data(), vector_size_bytes, &wr_req, &ret));

    wr_req.priv_data = (void *)"write_c";
    // Writing data to input stream 2 independently in case of non-blocking transfers.
    OCL_CHECK(
        ret,
        xcl::Stream::writeStream(
            write_stream_c, h_c.data(), vector_size_bytes, &wr_req, &ret));

    // Initiating the READ transfer
    cl_stream_xfer_req rd_req{0};
    rd_req.flags = CL_STREAM_EOT | CL_STREAM_NONBLOCKING;
    rd_req.priv_data = (void *)"read";
    // Read the stream data independently in case of non-blocking transfers.
    OCL_CHECK(
        ret,
        xcl::Stream::readStream(
            read_stream, hw_results.data(), vector_size_bytes, &rd_req, &ret));

    // Checking the request completion
    cl_streams_poll_req_completions poll_req[4]{0, 0, 0, 0}; // 4 Requests
    auto num_compl = 4;
    OCL_CHECK(ret,
              xcl::Stream::pollStreams(
                  device.get(), poll_req, 4, 4, &num_compl, 50000, &ret));
    // Blocking API, waits for 4 poll request completion or 50000ms, whichever occurs first.

    // Ensuring all OpenCL objects are released.
    q.finish();

    // Compare the device results with software results
    bool match = verify(sw_results.data(), hw_results.data(), size);

    // Releasing Streams
    xcl::Stream::releaseStream(read_stream);
    xcl::Stream::releaseStream(write_stream_a);
    xcl::Stream::releaseStream(write_stream_b);
    xcl::Stream::releaseStream(write_stream_c);
    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
