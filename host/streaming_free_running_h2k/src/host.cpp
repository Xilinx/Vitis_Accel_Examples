/**********
Copyright (c) 2020, Xilinx, Inc.
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
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/
#include "cmdlineparser.h"
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <string>
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

////////////////////RESET FUNCTION//////////////////////////////////
int reset(int *a, int *sw_results, int *hw_results, unsigned int size) {
  // Fill the input vectors with data
  std::generate(a, a + size, std::rand);
  for (size_t i = 0; i < size; i++) {
    sw_results[i] = a[i] + 1;
  }
  return 0;
}
///////////////////VERIFY FUNCTION///////////////////////////////////
bool verify(int *sw_results, int *hw_results, int size) {
  bool match = true;
  for (int i = 0; i < size; i++) {
    if (sw_results[i] != hw_results[i]) {
      match = false;
      std::cout << i << "\n" << sw_results[i] << "\n" << hw_results[i] << " \n";
      break;
    }
  }
  std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;
  return match;
}
////////MAIN FUNCTION//////////
int main(int argc, char **argv) {

  std::string Block_Count = "1024";
  if (xcl::is_emulation()) {
    Block_Count = "2";
  }

  // Command Line Parser
  sda::utils::CmdLineParser parser;

  // Switches
  //**************//"<Full Arg>",  "<Short Arg>", "<Description>", "<Default>"
  parser.addSwitch("--xclbin_file", "-x", "input binary file string", "");
  parser.addSwitch("--block_count", "-nb", "number of blocks", Block_Count);
  parser.addSwitch("--block_size", "-bs", "Size of each block in KB", "256");
  parser.parse(argc, argv);

  // Read settings
  std::string binaryFile = parser.value("xclbin_file");
  unsigned int num_Blocks = stoi(parser.value("block_count"));
  unsigned int Block_Size = stoi(parser.value("block_size"));

  if (binaryFile.empty()) {
    parser.printHelp();
    exit(EXIT_FAILURE);
  }

  Block_Size *= 1000;

  unsigned int size = num_Blocks * Block_Size;

  // I/O Data Vectors
  std::vector<int, aligned_allocator<int>> h_a(size);
  std::vector<int, aligned_allocator<int>> hw_results(size);
  std::vector<int> sw_results(size);

  reset(h_a.data(), sw_results.data(), hw_results.data(), size);

  std::cout << "\n Vector Addition of elements " << size << std::endl;

  // Bytes per Block
  int vector_size_bytes = sizeof(int) * Block_Size;

  // OpenCL Host Code Begins
  cl_int err;
  cl::Kernel increment;
  int no_of_elem = Block_Size;
  cl::CommandQueue q;
  cl::Context context;
  cl::Device device;
  // get_xil_devices() is a utility API which will find the xilinx
  // platforms and will return list of devices connected to Xilinx platform
  auto devices = xcl::get_xil_devices();

  // read_binary_file() is a utility API which will load the binaryFile
  // and will return the pointer to file buffer.
  auto fileBuf = xcl::read_binary_file(binaryFile);
  cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
  bool valid_device = false;
  for (unsigned int i = 0; i < devices.size(); i++) {
    device = devices[i];
    // Creating Context and Command Queue for selected Device
    OCL_CHECK(err, context = cl::Context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(err,
              q = cl::CommandQueue(context, device,
                                   CL_QUEUE_PROFILING_ENABLE |
                                       CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE,
                                   &err));

    std::cout << "Trying to program device[" << i
              << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
    cl::Program program(context, {device}, bins, NULL, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
    } else {
      std::cout << "Device[" << i << "]: program successful!\n";
      // Creating Kernel

      OCL_CHECK(err, increment = cl::Kernel(program, "increment", &err));
      valid_device = true;
      break; // we break because we found a valid device
    }
  }
  if (!valid_device) {
    std::cout << "Failed to program any device found, exit!\n";
    exit(EXIT_FAILURE);
  }

  auto platform_id = device.getInfo<CL_DEVICE_PLATFORM>(&err);

  // Initialization of streaming class is needed before using it.
  xcl::Stream::init(platform_id);

  // Streams
  cl_stream h2c_Stream_a;
  cl_stream c2h_Stream;

  cl_int ret;

  // Device Connection specification of the stream through extension pointer
  cl_mem_ext_ptr_t ext;
  ext.param = increment.get();
  ext.obj = NULL;
  // The .flag should be used to denote the kernel argument
  // Create write stream for argument 0 of kernel
  ext.flags = 0;
  OCL_CHECK(ret,
            h2c_Stream_a = xcl::Stream::createStream(
                device.get(), XCL_STREAM_READ_ONLY, CL_STREAM, &ext, &ret));

  // Create read stream for argument 1 of kernel
  ext.flags = 1;
  OCL_CHECK(ret, c2h_Stream = xcl::Stream::createStream(device.get(),
                                                        XCL_STREAM_WRITE_ONLY,
                                                        CL_STREAM, &ext, &ret));

  // Sync for the async streaming
  int num_compl = 2 * num_Blocks;

  // Checking the request completions
  cl_streams_poll_req_completions *poll_req;
  poll_req = (cl_streams_poll_req_completions *)malloc(
      sizeof(cl_streams_poll_req_completions) * num_compl);
  memset(poll_req, 0, sizeof(cl_streams_poll_req_completions) * num_compl);

  cl_stream_xfer_req rd_req{0};
  cl_stream_xfer_req wr_req{0};

  rd_req.flags = CL_STREAM_EOT | CL_STREAM_NONBLOCKING;
  wr_req.flags = CL_STREAM_EOT | CL_STREAM_NONBLOCKING;

  auto total_start = std::chrono::high_resolution_clock::now();
  for (unsigned int i = 0; i < num_Blocks; i++) {

    auto write_tag_a = "write_a_" + std::to_string(i);
    wr_req.priv_data = (void *)write_tag_a.c_str();

    std::cout << "\n Writing Block h2c_Stream_a[" << i << "]";
    OCL_CHECK(ret, xcl::Stream::writeStream(h2c_Stream_a,
                                            (h_a.data() + i * no_of_elem),
                                            vector_size_bytes, &wr_req, &ret));

    auto read_tag = "read_" + std::to_string(i);
    rd_req.priv_data = (void *)read_tag.c_str();

    std::cout << "\n Reading Block c2h_Stream[" << i << "]";
    OCL_CHECK(ret, xcl::Stream::readStream(c2h_Stream,
                                           (hw_results.data() + i * no_of_elem),
                                           vector_size_bytes, &rd_req, &ret));
  }

  // Checking the request completions
  std::cout << "\n clPollStreams for (" << num_compl
            << ") events (Blocks: " << num_Blocks
            << ", axis_in: 1, axis_out: 1)\n";
  OCL_CHECK(ret, xcl::Stream::pollStreams(device.get(), poll_req, num_compl,
                                          num_compl, &num_compl, 50000, &ret));
  q.finish();

  auto total_end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration<double, std::nano>(total_end - total_start);
  double throput =
      ((double)size * sizeof(int) * 2) /
      (double)
          duration.count(); // Multiplied by 2 because read and write both done
  std::cout << "Throughput = " << throput << " GB/s\n";

  // Compare the device results with software results
  bool match = verify(sw_results.data(), hw_results.data(), size);

  // Releasing all OpenCL objects
  xcl::Stream::releaseStream(c2h_Stream);
  xcl::Stream::releaseStream(h2c_Stream_a);

  return match ? EXIT_SUCCESS : EXIT_FAILURE;
}
