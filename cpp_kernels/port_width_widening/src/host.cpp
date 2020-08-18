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

#include <algorithm>
#include <iostream>
#include <random>
#include <vector>

constexpr int DATA_SIZE = 512;

// Compare the results of the Device to the simulation
void verify(const std::vector<int, aligned_allocator<int>> &gold,
            const std::vector<int, aligned_allocator<int>> &out) {
  bool match = true;
  for (int i = 0; i < DATA_SIZE; i++) {
    if (out[i] != gold[i]) {
      std::cout << "Error: Result mismatch" << std::endl;
      std::cout << "i = " << i << " CPU result = " << gold[i]
                << " Device result = " << out[i] << std::endl;
      match = false;
      break;
    }
  }
  if (!match) {
    std::cout << "TEST FAILED" << std::endl;
    exit(EXIT_FAILURE);
  }
}

void loop_sequential_sw(const std::vector<int, aligned_allocator<int>> &A,
                        const std::vector<int, aligned_allocator<int>> &B,
                        std::vector<int, aligned_allocator<int>> &RES) {

  int X_accum = 0;
  int Y_accum = 0;
  int i;
  int X_aux[DATA_SIZE];
  int y_aux[DATA_SIZE];

  for (i = 0; i < DATA_SIZE; i++) {
    X_accum += A[i];
    X_aux[i] = X_accum;
  }

  for (i = 0; i < DATA_SIZE; i++) {
    Y_accum += B[i];
    y_aux[i] = Y_accum;
  }

  for (i = 0; i < DATA_SIZE; i++) {
    RES[i] = X_aux[i] * y_aux[i];
  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
    return EXIT_FAILURE;
  }

  std::string binaryFile = argv[1];

  // compute the size of array in bytes
  size_t size_in_bytes = DATA_SIZE * sizeof(int);
  cl_int err;
  cl::CommandQueue q;
  cl::Context context;
  cl::Program program;

  /* less iteration for emulation mode */
  int iteration = xcl::is_emulation() ? 2 : 1000;

  std::vector<int, aligned_allocator<int>> source_a(DATA_SIZE);
  std::vector<int, aligned_allocator<int>> source_b(DATA_SIZE);
  std::vector<int, aligned_allocator<int>> source_sw_results(DATA_SIZE);
  std::vector<int, aligned_allocator<int>> source_a_sw(DATA_SIZE);
  std::vector<int, aligned_allocator<int>> source_b_sw(DATA_SIZE);
  std::vector<int, aligned_allocator<int>> source_hw_results(DATA_SIZE);

  // Create the test data
  std::generate(source_a.begin(), source_a.end(), std::rand);
  std::generate(source_b.begin(), source_b.end(), std::rand);
  for (int i = 0; i < DATA_SIZE; i++) {
    source_a_sw[i] = source_a[i];
    source_b_sw[i] = source_b[i];
    source_sw_results[i] = 0;
    source_hw_results[i] = 0;
  }

  // Getting Xilinx Device and creating program and context
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
    program = cl::Program(context, {device}, bins, NULL, &err);
    if (err != CL_SUCCESS) {
      std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
    } else {
      std::cout << "Device[" << i << "]: program successful!\n";
      valid_device = true;
      break; // we break because we found a valid device
    }
  }
  if (!valid_device) {
    std::cout << "Failed to program any device found, exit!\n";
    exit(EXIT_FAILURE);
  }

  loop_sequential_sw(source_a, source_b, source_sw_results);

  // Allocate Buffer in Global Memory
  OCL_CHECK(err,
            cl::Buffer buffer_a(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                size_in_bytes, source_a.data(), &err));
  OCL_CHECK(err,
            cl::Buffer buffer_b(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                size_in_bytes, source_b.data(), &err));
  OCL_CHECK(err, cl::Buffer buffer_result(
                     context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                     size_in_bytes, source_hw_results.data(), &err));

  OCL_CHECK(err, cl::Kernel kernel_base(program, "krnl_base", &err));

  OCL_CHECK(err, err = kernel_base.setArg(0, buffer_a));
  OCL_CHECK(err, err = kernel_base.setArg(1, buffer_b));
  OCL_CHECK(err, err = kernel_base.setArg(2, buffer_result));

  // Copy input data to device global memory
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_a, buffer_b},
                                                  0 /* 0 means from host*/));
  auto total_start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < iteration; i++) {
    OCL_CHECK(err, err = q.enqueueTask(kernel_base));
  }
  auto total_end = std::chrono::high_resolution_clock::now();
  auto krnl_base_time_ns = std::chrono::duration<double, std::nano>(total_end - total_start);
  // Copy Result from Device Global Memory to Host Local Memory
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects(
                     {buffer_result}, CL_MIGRATE_MEM_OBJECT_HOST));
  q.finish();
  verify(source_sw_results, source_hw_results);

  OCL_CHECK(err, cl::Kernel kernel_widen(program, "krnl_widen", &err));

  OCL_CHECK(err, err = kernel_widen.setArg(0, buffer_a));
  OCL_CHECK(err, err = kernel_widen.setArg(1, buffer_b));
  OCL_CHECK(err, err = kernel_widen.setArg(2, buffer_result));

  total_start = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < iteration; i++) {
    OCL_CHECK(err, err = q.enqueueTask(kernel_widen));
  }
  total_end = std::chrono::high_resolution_clock::now();
  auto krnl_widen_time_ns = std::chrono::duration<double, std::nano>(total_end - total_start);
  // Copy Result from Device Global Memory to Host Local Memory
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects(
                     {buffer_result}, CL_MIGRATE_MEM_OBJECT_HOST));
  q.finish();
  verify(source_sw_results, source_hw_results);

  printf("|-------------------------+-------------------------|\n"
         "| Kernel(%3d iterations)  |  Wall-Clock Time (ms) |\n"
         "|-------------------------+-------------------------|\n",
         iteration);
  std::cout << "| krnl_base \t\t  |\t" << krnl_base_time_ns.count() / 1000000 << std::endl;
  std::cout << "| krnl_widen\t\t  |\t" << krnl_widen_time_ns.count() / 1000000 << std::endl;
  std::cout << "|-------------------------+-------------------------|"
            << std::endl;
  std::cout
      << "Note: Wall Clock Time is meaningful for real hardware execution "
         "only, not for emulation."
      << std::endl;
  std::cout << "TEST PASSED" << std::endl;
  return EXIT_SUCCESS;
}
