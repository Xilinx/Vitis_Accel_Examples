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
#include "xcl2.hpp"
#include <vector>

#define DATA_SIZE 256

void wait_for_enter(const std::string &msg) {
  std::cout << msg << std::endl;
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void invokeVivadoDebugScript(const std::string &msg) {
  std::cout << msg << std::endl;
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
    return EXIT_FAILURE;
  }

  std::string binaryFile = argv[1];

  auto size = DATA_SIZE;
  // Allocate Memory in Host Memory
  auto vector_size_bytes = sizeof(int) * size;
  std::vector<int, aligned_allocator<int>> source_input1(size);
  std::vector<int, aligned_allocator<int>> source_input2(size);
  std::vector<int, aligned_allocator<int>> source_hw_results(size);
  std::vector<int, aligned_allocator<int>> source_sw_results(size);

  // Create the test data and Software Result
  for (int i = 0; i < size; i++) {
    source_input1[i] = i;
    source_input2[i] = i;
    source_sw_results[i] = source_input1[i] + source_input2[i];
    source_hw_results[i] = 0;
  }

  // OPENCL HOST CODE AREA START
  // Create Program and Kernel
  cl_int err;
  cl::CommandQueue q;
  cl::Context context;
  cl::Kernel krnl_vadd;
  auto devices = xcl::get_xil_devices();

  // read_binary_file() is a utility API which will load the binaryFile
  // and will return the pointer to file buffer.
  auto fileBuf = xcl::read_binary_file(binaryFile);
  cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
  bool valid_device = false;
  for (unsigned int i = 0; i < devices.size(); i++) {
    auto device = devices[i];
    // Creating Context and Command Queue for selected Device
    OCL_CHECK(err, context = cl::Context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(err, q = cl::CommandQueue(context, device,
                                        CL_QUEUE_PROFILING_ENABLE, &err));

    std::cout << "Trying to program device[" << i
              << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
    cl::Program program(context, {device}, bins, NULL, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
    } else {
      std::cout << "Device[" << i << "]: program successful!\n";
      OCL_CHECK(err, krnl_vadd = cl::Kernel(program, "krnl_vadd_rtl", &err));
      valid_device = true;
      break; // we break because we found a valid device
    }
  }
  if (!valid_device) {
    std::cout << "Failed to program any device found, exit!\n";
    exit(EXIT_FAILURE);
  }

  // if (interactive == true)
  // wait_for_enter("\nPress ENTER to continue after setting up ILA
  // trigger...");
  // else
  // call script/socket to invoke Vivado, connect to server, XVC target, set
  // probe files, setup ILA triggers and arm ILA
  // invokeVivadoDebugScript();

  // Allocate Buffer in Global Memory
  OCL_CHECK(
      err, cl::Buffer buffer_r1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                vector_size_bytes, source_input1.data(), &err));
  OCL_CHECK(
      err, cl::Buffer buffer_r2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                vector_size_bytes, source_input2.data(), &err));
  OCL_CHECK(err, cl::Buffer buffer_w(
                     context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                     vector_size_bytes, source_hw_results.data(), &err));

  // Set the Kernel Arguments
  OCL_CHECK(err, err = krnl_vadd.setArg(0, buffer_r1));
  OCL_CHECK(err, err = krnl_vadd.setArg(1, buffer_r2));
  OCL_CHECK(err, err = krnl_vadd.setArg(2, buffer_w));
  OCL_CHECK(err, err = krnl_vadd.setArg(3, size));

  // Copy input data to device global memory
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_r1, buffer_r2},
                                                  0 /* 0 means from host*/));

  // Launch the Kernel
  OCL_CHECK(err, err = q.enqueueTask(krnl_vadd));

  // Copy Result from Device Global Memory to Host Local Memory
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_w},
                                                  CL_MIGRATE_MEM_OBJECT_HOST));
  OCL_CHECK(err, err = q.finish());

  // OPENCL HOST CODE AREA END

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
