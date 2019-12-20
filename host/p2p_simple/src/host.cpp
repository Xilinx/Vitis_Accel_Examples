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

// OpenCL utility layer include
#include "xcl2.hpp"
#include <fcntl.h>
#include <fstream>
#include <iomanip>
#include <iosfwd>
#include <iostream>
#include <unistd.h>
#include <vector>

using std::ifstream;
using std::ios;
using std::streamsize;

#define DATA_SIZE 4096
#define INCR_VALUE 10

std::vector<unsigned char> readBinary(const std::string &fileName) {
  ifstream file(fileName, ios::binary | ios::ate);
  if (file) {
    file.seekg(0, ios::end);
    streamsize size = file.tellg();
    file.seekg(0, ios::beg);
    std::vector<unsigned char> buffer(size);
    file.read((char *)buffer.data(), size);
    return buffer;
  } else {
    return std::vector<unsigned char>(0);
  }
}

void p2p_host_to_ssd(int &nvmeFd, cl_context context, cl_command_queue q,
                     cl_program program,
                     std::vector<int, aligned_allocator<int>> source_input_A) {
  uint32_t *p2pPtr;
  cl_int error;
  int err;
  int ret = 0, inc = INCR_VALUE;
  int size = DATA_SIZE;
  size_t vector_size_bytes = sizeof(int) * DATA_SIZE;

  cl_kernel krnl_adder;
  // Allocate Buffer in Global Memory
  cl_mem_ext_ptr_t outExt;
  outExt = {XCL_MEM_EXT_P2P_BUFFER, NULL, 0};

  cl_mem input_a =
      clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                     vector_size_bytes, source_input_A.data(), &err);
  cl_mem p2pBo =
      clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_EXT_PTR_XILINX,
                     vector_size_bytes, &outExt, &err);

  krnl_adder = clCreateKernel(program, "adder", &err);
  // Set the Kernel Arguments
  int narg = 0;
  clSetKernelArg(krnl_adder, narg++, sizeof(cl_mem), &input_a);
  clSetKernelArg(krnl_adder, narg++, sizeof(cl_mem), &p2pBo);
  clSetKernelArg(krnl_adder, narg++, sizeof(int), &inc);
  clSetKernelArg(krnl_adder, narg++, sizeof(int), &size);

  // Copy input data to device global memory
  clEnqueueMigrateMemObjects(q, 1, &input_a, 0, 0, nullptr, nullptr);

  // Launch the Kernel
  OCL_CHECK(error, error = clEnqueueTask(q, krnl_adder, 0, NULL, NULL));

  std::cout << "\nMap P2P device buffers to host access pointers\n"
            << std::endl;
  p2pPtr = (uint32_t *)clEnqueueMapBuffer(
      q, p2pBo, CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, 0, vector_size_bytes, 0,
      NULL, NULL, NULL);

  std::cout << "Now start P2P Write from device buffers to SSD\n" << std::endl;
  ret = pwrite(nvmeFd, (void *)p2pPtr, vector_size_bytes, 0);
  if (ret == -1)
    std::cout << "P2P: write() failed, err: " << ret << ", line: " << __LINE__
              << std::endl;

  std::cout << "Clean up the buffers\n" << std::endl;
  clReleaseKernel(krnl_adder);
  clReleaseMemObject(input_a);
  clReleaseMemObject(p2pBo);
}

void p2p_ssd_to_host(
    int &nvmeFd, cl_context context, cl_command_queue q, cl_program program,
    std::vector<int, aligned_allocator<int>> *source_hw_results) {
  uint32_t *p2pPtr1;
  cl_int error;
  int err;
  int inc = INCR_VALUE;
  int size = DATA_SIZE;
  size_t vector_size_bytes = sizeof(int) * DATA_SIZE;

  cl_kernel krnl_adder1;
  // Allocate Buffer in Global Memory
  cl_mem_ext_ptr_t inExt;
  inExt = {XCL_MEM_EXT_P2P_BUFFER, NULL, 0};

  cl_mem buffer_input =
      clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX,
                     vector_size_bytes, &inExt, &err);
  cl_mem buffer_output =
      clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
                     vector_size_bytes, source_hw_results->data(), &err);

  krnl_adder1 = clCreateKernel(program, "adder", &err);
  std::cout << "\nMap P2P device buffers to host access pointers\n"
            << std::endl;
  p2pPtr1 =
      (uint32_t *)clEnqueueMapBuffer(q, buffer_input, CL_TRUE, CL_MAP_READ, 0,
                                     vector_size_bytes, 0, NULL, NULL, NULL);

  std::cout << "Now start P2P Read from SSD to device buffers\n" << std::endl;
  if (pread(nvmeFd, (void *)p2pPtr1, vector_size_bytes, 0) <= 0) {
    std::cout << "ERR: pread failed: "
              << " error: " << strerror(errno) << std::endl;
    exit(EXIT_FAILURE);
  }

  // Set the Kernel Arguments
  int narg = 0;
  clSetKernelArg(krnl_adder1, narg++, sizeof(cl_mem), &buffer_input);
  clSetKernelArg(krnl_adder1, narg++, sizeof(cl_mem), &buffer_output);
  clSetKernelArg(krnl_adder1, narg++, sizeof(int), &inc);
  clSetKernelArg(krnl_adder1, narg++, sizeof(int), &size);

  // Launch the Kernel
  OCL_CHECK(error, error = clEnqueueTask(q, krnl_adder1, 0, NULL, NULL));

  // Read output data to host
  clEnqueueReadBuffer(q, buffer_output, CL_TRUE, 0, vector_size_bytes,
                      static_cast<void *>(source_hw_results->data()), 0, NULL,
                      NULL);

  std::cout << "Clean up the buffers\n" << std::endl;
  clReleaseKernel(krnl_adder1);
  clReleaseMemObject(buffer_input);
  clReleaseMemObject(buffer_output);
}

int main(int argc, char **argv) {
  if (argc != 3) {
    std::cout << "Usage: " << argv[0] << " <XCLBIN File>"
              << "<Path to SSD>" << std::endl;
    return EXIT_FAILURE;
  }

  auto binaryFile = argv[1];
  char *filename;
  filename = argv[2];
  int nvmeFd = -1;

  // Allocate Memory in Host Memory
  cl_int error;
  cl_program program;
  cl_context context;
  cl_command_queue q;

  int err;
  cl_device_id device;
  cl_platform_id platform;
  cl_uint num_platforms;
  err = clGetPlatformIDs(0, NULL, &num_platforms);
  cl_platform_id *platform_ids =
      (cl_platform_id *)malloc(sizeof(cl_platform_id) * num_platforms);
  err = clGetPlatformIDs(num_platforms, platform_ids, NULL);
  size_t i;
  device = 0;

  for (i = 0; i < num_platforms; i++) {
    size_t platform_name_size;
    err = clGetPlatformInfo(platform_ids[i], CL_PLATFORM_NAME, 0, NULL,
                            &platform_name_size);
    if (err != CL_SUCCESS) {
      printf("Error: Could not determine platform name!\n");
      exit(EXIT_FAILURE);
    }

    char *platform_name = (char *)malloc(sizeof(char) * platform_name_size);
    if (platform_name == NULL) {
      printf("Error: out of memory!\n");
      exit(EXIT_FAILURE);
    }

    err = clGetPlatformInfo(platform_ids[i], CL_PLATFORM_NAME,
                            platform_name_size, platform_name, NULL);
    if (err != CL_SUCCESS) {
      printf("Error: could not determine platform name!\n");
      exit(EXIT_FAILURE);
    }

    if (!strcmp(platform_name, "Xilinx")) {
      free(platform_name);
      platform = platform_ids[i];
      break;
    }

    free(platform_name);
  }

  err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, NULL);
  cl_context_properties properties[3] = {CL_CONTEXT_PLATFORM,
                                         (cl_context_properties)platform, 0};

  context = clCreateContext(properties, 1, &device, NULL, NULL, &err);
  if (err != CL_SUCCESS)
    std::cout << "clCreateContext call: Failed to create a compute context"
              << err << std::endl;

  std::vector<unsigned char> binary = readBinary(binaryFile);
  size_t binary_size = binary.size();
  const unsigned char *binary_data = binary.data();

  program = clCreateProgramWithBinary(context, 1, &device, &binary_size,
                                      &binary_data, NULL, &err);

  q = clCreateCommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &error);

  std::vector<int, aligned_allocator<int>> source_input_A(DATA_SIZE, 15);
  std::vector<int, aligned_allocator<int>> source_sw_results(DATA_SIZE);
  std::vector<int, aligned_allocator<int>> source_hw_results(DATA_SIZE);

  // Create the test data and Software Result
  for (int i = 0; i < DATA_SIZE; i++) {
    source_sw_results[i] = source_input_A[i] + 2 * INCR_VALUE;
  }

  // P2P transfer from host to SSD
  std::cout << "############################################################\n";
  std::cout << "                  Writing data to SSD                       \n";
  std::cout << "############################################################\n";
  // Get access to the NVMe SSD.
  nvmeFd = open(filename, O_RDWR | O_DIRECT);
  if (nvmeFd < 0) {
    std::cout << "ERROR: open " << filename << "failed: " << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "INFO: Successfully opened NVME SSD " << filename << std::endl;
  p2p_host_to_ssd(nvmeFd, context, q, program, source_input_A);
  (void)close(nvmeFd);

  // P2P transfer from SSD to host
  std::cout << "############################################################\n";
  std::cout
      << "                  Reading data from SSD                       \n";
  std::cout << "############################################################\n";

  nvmeFd = open(filename, O_RDWR | O_DIRECT);
  if (nvmeFd < 0) {
    std::cout << "ERROR: open " << filename << "failed: " << std::endl;
    return EXIT_FAILURE;
  }
  std::cout << "INFO: Successfully opened NVME SSD " << filename << std::endl;

  bool num_matched = true;
  p2p_ssd_to_host(nvmeFd, context, q, program, &source_hw_results);

  // Validating the results
  if (memcmp(static_cast<void *>(source_sw_results.data()),
             static_cast<void *>(source_hw_results.data()), DATA_SIZE) != 0) {
    num_matched = false;
  }

  (void)close(nvmeFd);

  // Release rest of OpenCL setup
  clReleaseCommandQueue(q);
  clReleaseContext(context);
  clReleaseProgram(program);

  std::cout << "TEST " << (num_matched ? "PASSED" : "FAILED") << std::endl;
  return (num_matched ? EXIT_SUCCESS : EXIT_FAILURE);
}
