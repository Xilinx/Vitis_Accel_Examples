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

/*
   Shift Register

   This example demonstrates how to perform a shift register operation to
   implement a Finite Impulse Response(FIR) filter.

   NOTE: See the fir.cl file for additional information.
  */
#include "xcl2.hpp"

#include <algorithm>
#include <random>
#include <string>
#include <vector>

#define SIGNAL_SIZE (1024 * 1024)
#define SIGNAL_SIZE_IN_EMU 1024

using std::default_random_engine;
using std::inner_product;
using std::string;
using std::uniform_int_distribution;
using std::vector;

// helping functions
void fir_sw(vector<int, aligned_allocator<int>> &output,
            const vector<int, aligned_allocator<int>> &signal,
            const vector<int, aligned_allocator<int>> &coeff);

void verify(const vector<int, aligned_allocator<int>> &gold,
            const vector<int, aligned_allocator<int>> &out);
uint64_t get_duration_ns(const cl::Event &event);
void print_summary(std::string k1, std::string k2, uint64_t t1, uint64_t t2,
                   int iterations);
int gen_random();

int main(int argc, char **argv) {

  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
    return EXIT_FAILURE;
  }

  std::string binaryFile = argv[1];
  int signal_size = xcl::is_emulation() ? SIGNAL_SIZE_IN_EMU : SIGNAL_SIZE;
  vector<int, aligned_allocator<int>> signal(signal_size);
  vector<int, aligned_allocator<int>> out(signal_size);
  vector<int, aligned_allocator<int>> coeff = {
      {53, 0, -91, 0, 313, 500, 313, 0, -91, 0, 53}};
  vector<int, aligned_allocator<int>> gold(signal_size, 0);
  generate(begin(signal), end(signal), gen_random);

  fir_sw(gold, signal, coeff);

  size_t size_in_bytes = signal_size * sizeof(int);
  size_t coeff_size_in_bytes = coeff.size() * sizeof(int);
  cl_int err;
  cl::CommandQueue q;
  cl::Context context;
  cl::Program program;

  // Initialize OpenCL context and load xclbin binary
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

  // Allocate Buffer in Global Memory
  OCL_CHECK(err, cl::Buffer buffer_signal(context, CL_MEM_USE_HOST_PTR |
                                                       CL_MEM_READ_ONLY,
                                          size_in_bytes, signal.data(), &err));
  OCL_CHECK(err, cl::Buffer buffer_coeff(
                     context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                     coeff_size_in_bytes, coeff.data(), &err));
  OCL_CHECK(err, cl::Buffer buffer_output(context, CL_MEM_USE_HOST_PTR |
                                                       CL_MEM_WRITE_ONLY,
                                          size_in_bytes, out.data(), &err));

  // Creating Naive Kernel Object and setting args
  OCL_CHECK(err, cl::Kernel fir_naive_kernel(program, "fir_naive", &err));

  OCL_CHECK(err, err = fir_naive_kernel.setArg(0, buffer_output));
  OCL_CHECK(err, err = fir_naive_kernel.setArg(1, buffer_signal));
  OCL_CHECK(err, err = fir_naive_kernel.setArg(2, buffer_coeff));
  OCL_CHECK(err, err = fir_naive_kernel.setArg(3, signal_size));

  // Copy input data to device global memory
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_signal, buffer_coeff},
                                                  0 /* 0 means from host*/));

  cl::Event event;
  int iterations = xcl::is_emulation() ? 2 : 100;
  uint64_t fir_naive_time = 0;
  // Running naive kernel iterations times
  for (int i = 0; i < iterations; i++) {
    OCL_CHECK(err, err = q.enqueueTask(fir_naive_kernel, NULL, &event));
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects(
                       {buffer_output}, CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();
    fir_naive_time += get_duration_ns(event);
    verify(gold, out);
  }

  // Creating FIR Shift Register Kernel object and setting args
  OCL_CHECK(err, cl::Kernel fir_sr_kernel(program, "fir_shift_register", &err));

  OCL_CHECK(err, err = fir_sr_kernel.setArg(0, buffer_output));
  OCL_CHECK(err, err = fir_sr_kernel.setArg(1, buffer_signal));
  OCL_CHECK(err, err = fir_sr_kernel.setArg(2, buffer_coeff));
  OCL_CHECK(err, err = fir_sr_kernel.setArg(3, signal_size));

  uint64_t fir_sr_time = 0;
  // Running Shift Register FIR iterations times
  for (int i = 0; i < iterations; i++) {
    OCL_CHECK(err, err = q.enqueueTask(fir_sr_kernel, NULL, &event));
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects(
                       {buffer_output}, CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();
    fir_sr_time += get_duration_ns(event);
    verify(gold, out);
  }
  printf("Example Testdata Signal_Length=%u for %d iteration\n", signal_size,
         iterations);
  print_summary("fir_naive", "fir_shift_register", fir_naive_time, fir_sr_time,
                iterations);
  printf("TEST PASSED\n");
  return EXIT_SUCCESS;
}

// Finite Impulse Response Filter
void fir_sw(vector<int, aligned_allocator<int>> &output,
            const vector<int, aligned_allocator<int>> &signal,
            const vector<int, aligned_allocator<int>> &coeff) {
  auto out_iter = begin(output);
  auto rsignal_iter = signal.rend() - 1;

  int i = 0;
  while (rsignal_iter != signal.rbegin() - 1) {
    int elements = std::min((int)coeff.size(), i++);
    *(out_iter++) =
        inner_product(begin(coeff), begin(coeff) + elements, rsignal_iter--, 0);
  }
}

int gen_random() {
  static default_random_engine e;
  static uniform_int_distribution<int> dist(0, 100);

  return dist(e);
}

// Verifies the gold and the out data are equal
void verify(const vector<int, aligned_allocator<int>> &gold,
            const vector<int, aligned_allocator<int>> &out) {
  bool match = equal(begin(gold), end(gold), begin(out));
  if (!match) {
    printf("TEST FAILED\n");
    exit(EXIT_FAILURE);
  }
}

uint64_t get_duration_ns(const cl::Event &event) {
  uint64_t nstimestart, nstimeend;
  cl_int err;
  OCL_CHECK(err, err = event.getProfilingInfo<uint64_t>(
                     CL_PROFILING_COMMAND_START, &nstimestart));
  OCL_CHECK(err, err = event.getProfilingInfo<uint64_t>(
                     CL_PROFILING_COMMAND_END, &nstimeend));
  return (nstimeend - nstimestart);
}
void print_summary(std::string k1, std::string k2, uint64_t t1, uint64_t t2,
                   int iterations) {
  double speedup = (double)t1 / (double)t2;
  printf("|-------------------------+-------------------------|\n"
         "| Kernel(%3d iterations)  |    Wall-Clock Time (ns) |\n"
         "|-------------------------+-------------------------|\n",
         iterations);
  printf("| %-23s | %23lu |\n", k1.c_str(), t1);
  printf("| %-23s | %23lu |\n", k2.c_str(), t2);
  printf("|-------------------------+-------------------------|\n");
  printf("| Speedup: | %23lf |\n", speedup);
  printf("|-------------------------+-------------------------|\n");
  printf("Note: Wall Clock Time is meaningful for real hardware execution "
         "only, not for emulation.\n");
  printf("Please refer to profile summary for kernel execution time for "
         "hardware emulation.\n");

  // Performance check for real hardware. t2 must be less than t1.
  if (!xcl::is_emulation() && (t1 < t2)) {
    printf("ERROR: Unexpected Performance is observed\n");
    exit(EXIT_FAILURE);
  }
}
