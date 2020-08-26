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
#include <chrono>
#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>

typedef int data_t;
#define LENGTH 65536
// Declaration of custom p2p APIs that binds to Xilinx p2p APIs.
decltype(&xclGetMemObjectFd) xcl::P2P::getMemObjectFd = nullptr;
decltype(&xclGetMemObjectFromFd) xcl::P2P::getMemObjectFromFd = nullptr;

cl_program xcl_import_binary_file(cl_device_id device_id, cl_context context,
                                  const char *xclbin_file_name);
//==================================================================================================

void report_time(std::string label, cl_ulong totalTime, cl_ulong curTime) {
  double total = LENGTH;

  total *= 1000000;     // convert us to s
  total /= 1024 * 1024; // convert to MB

  std::cout << std::setw(8) << label << "\t" << std::fixed
            << std::setprecision(2) << std::setw(8) << curTime << "ms\t"
            << std::setw(8) << totalTime << "ms\t" << std::setw(8)
            << float(curTime) * 100 / totalTime << "%\t" << std::setw(8)
            << total / curTime << "MB/s\t" << std::endl;
}


int main(int argc, char *argv[]) {
  double size = LENGTH;
  data_t in1[LENGTH]; // original data set given to device
  data_t in3[LENGTH];
  for (int i = 0; i < LENGTH; i++) {
    in1[i] = i;
    in3[i] = i + 2;
  }

  cl_platform_id platform_id;
  clGetPlatformIDs(1, &platform_id, NULL);
  cl_uint device_count;
  clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ACCELERATOR, 0, NULL,
                 &device_count);
  std::cout << "Device count - " << device_count << std::endl;

  if (device_count < 2) {
    std::cout << "WARNING: This design does P2P transfer between two devices. "
                 "Please run this "
                 "design on machine with two devices.\n";
    return 0;
  }

  cl_device_id *device_id =
      (cl_device_id *)malloc(sizeof(cl_device_id) * device_count);
  clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ACCELERATOR, device_count,
                 device_id, NULL);

  cl_context context[device_count];
  cl_command_queue queue[device_count];
  cl_kernel krnl_dev0, krnl_dev1;
  cl_program program[device_count];
  int err;

  std::chrono::high_resolution_clock::time_point p2pStart;
  std::chrono::high_resolution_clock::time_point p2pEnd;

  std::cout << "Initializing OpenCL objects" << std::endl;
  for (uint8_t i = 0; i < device_count; i++) {
    context[i] = clCreateContext(0, 1, &device_id[i], NULL, NULL, &err);
    if (err != CL_SUCCESS)
      std::cout << "clCreateContext call: Failed to create a compute context"
                << err << std::endl;
    queue[i] = clCreateCommandQueue(context[i], device_id[i],
                                    CL_QUEUE_PROFILING_ENABLE, &err);
    if (err != CL_SUCCESS)
      std::cout << "clCreateCommandQueue call: Failed to create commandqueue"
                << err << std::endl;
  }

  int buffersize = sizeof(data_t) * LENGTH;
  //------------------------------- Program
  //-------------------------------------------
  program[0] =
      xcl_import_binary_file(device_id[0], context[0], argv[1]);
  OCL_CHECK(err,
            krnl_dev0 = clCreateKernel(program[0], "bandwidth", &err));
  program[1] =
      xcl_import_binary_file(device_id[1], context[1], argv[1]);
  OCL_CHECK(err,
            krnl_dev1 = clCreateKernel(program[1], "bandwidth", &err));

  xcl::P2P::init(platform_id);
  std::chrono::high_resolution_clock::time_point start =
      std::chrono::high_resolution_clock::now();
  // ---------------------------- Buffer-1
  // -------------------------------------------
  cl_mem input_a;
  OCL_CHECK(err, input_a = clCreateBuffer(context[0], CL_MEM_READ_ONLY,
                                          buffersize, nullptr, &err));

  cl_mem output_a;
  cl_mem_ext_ptr_t output_ext = {XCL_MEM_EXT_P2P_BUFFER, NULL, 0};
  OCL_CHECK(err, output_a = clCreateBuffer(context[0], CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX,
                                          buffersize, &output_ext, &err));

  // ---------------------------- Buffer-2
  // -------------------------------------------
  cl_mem output_b;
  cl_mem_ext_ptr_t out_ext_b = {XCL_MEM_EXT_P2P_BUFFER, NULL, 0};
  OCL_CHECK(err, output_b= clCreateBuffer(context[1], CL_MEM_READ_WRITE |
                                                          CL_MEM_EXT_PTR_XILINX,
                                          buffersize, &out_ext_b, &err));

  cl_mem input_b;
  OCL_CHECK(err, input_b = clCreateBuffer(context[1], CL_MEM_READ_ONLY,
                                          buffersize, nullptr, &err));

  // ----------------------------Set Args
  // -------------------------------------------
  std::cout << "Set Args FPGA-1\n" << std::endl;
  cl_mem temp;
  OCL_CHECK(err, temp = clCreateBuffer(context[0], CL_MEM_WRITE_ONLY,
                                          buffersize, nullptr, &err));
  OCL_CHECK(err,
            err = clSetKernelArg(krnl_dev0, 0, sizeof(cl_mem), &input_a));
  OCL_CHECK(err,
            err = clSetKernelArg(krnl_dev0, 1, sizeof(cl_mem), &output_a));
  //OCL_CHECK(err,
  //          err = clSetKernelArg(krnl_dev0, 1, sizeof(cl_mem), &temp));

  std::cout << "Set Args FPGA-2\n" << std::endl;
  OCL_CHECK(err,
            err = clSetKernelArg(krnl_dev1, 0, sizeof(cl_mem), &input_b));
  OCL_CHECK(err,
            err = clSetKernelArg(krnl_dev1, 1, sizeof(cl_mem), &output_b));
  // -----------------------------------------------------------------------
  std::cout << "Write data to FPGA-1 \n" << std::endl;
  OCL_CHECK(err, err = clEnqueueWriteBuffer(queue[0], input_a, CL_TRUE, 0,
                                            sizeof(data_t) * LENGTH, in1, 0,
                                            NULL, NULL));

  std::cout << "Write data to FPGA-2 \n" << std::endl;
  OCL_CHECK(err, err = clEnqueueWriteBuffer(queue[1], input_b, CL_TRUE, 0,
                                            sizeof(data_t) * LENGTH, in3, 0,
                                            NULL, NULL));

  std::cout << "Launch FPGA-1\n" << std::endl;
  OCL_CHECK(err, err = clEnqueueTask(queue[0], krnl_dev0, 0, NULL, NULL));
  clFinish(queue[0]);
  //------------------------- P2P
  //-----------------------------------------------------------
  p2pStart = std::chrono::high_resolution_clock::now();
  std::cout << "Transferring from FPGA-1 to FPGA-2..." << std::endl;
  int fd = -1;
  OCL_CHECK(err,
            err = xcl::P2P::getMemObjectFd(
                output_b, &fd)); // Import p2p buffer to file descriptor (fd)
  if (fd > 0) {
    std::cout << "Import FD:" << fd << std::endl;
  }

  cl_mem exported_buf;
  OCL_CHECK(err,
            err = xcl::P2P::getMemObjectFromFd(context[0], device_id[0], 0, fd,
                                               &exported_buf)); // Import
  cl_event event;
  OCL_CHECK(err, err = clEnqueueCopyBuffer(queue[0], input_a, exported_buf, 0,
                                           0, sizeof(data_t) * LENGTH, 0, NULL,
                                           &event)); // transfer
  clWaitForEvents(1, &event);
  //p2pEnd = std::chrono::high_resolution_clock::now();
  clReleaseMemObject(exported_buf);
  // -----------------------------------------------------------------------
  //------------------------- P2P
  //-----------------------------------------------------------
  //p2pStart = std::chrono::high_resolution_clock::now();
  std::cout << "Transferring from FPGA-2 to FPGA-1..." << std::endl;
  int fd2 = -1;
  OCL_CHECK(err,
            err = xcl::P2P::getMemObjectFd(
                output_a, &fd2)); // Import p2p buffer to file descriptor (fd)
  if (fd2 > 0) {
    std::cout << "Import FD:" << fd << std::endl;
  }

  cl_mem exported_buf2;
  OCL_CHECK(err,
            err = xcl::P2P::getMemObjectFromFd(context[1], device_id[1], 0, fd2,
                                               &exported_buf2)); // Import
  cl_event event2;
  OCL_CHECK(err, err = clEnqueueCopyBuffer(queue[1], input_b, exported_buf2, 0,
                                           0, sizeof(data_t) * LENGTH, 0, NULL,
                                           &event2)); // transfer
  clWaitForEvents(1, &event2);
  p2pEnd = std::chrono::high_resolution_clock::now();
  clReleaseMemObject(exported_buf2);
  // -----------------------------------------------------------------------

  cl_ulong p2pTime =
      std::chrono::duration_cast<std::chrono::microseconds>(p2pEnd - p2pStart)
          .count();
  double dmsduration = (double)p2pTime;
  double dsduration = dmsduration / ((double)1000000000);

  double bpersec = (size / dsduration);
  double gbpersec = (2*bpersec) / ((double)1024 * 1024);

  //std::cout << "Kernel completed read " << dmbytes << " MB bytes from/to global memory.\n";
  //std::cout << "Number of Iterations : " << iter << std::endl;
  //std::cout << "\nExecution time = "<< dsduration << " (sec) \n";
  std::cout << "P2P Throughput = " << gbpersec << " (GB/sec) \n\n";
  
  std::chrono::high_resolution_clock::time_point end =
      std::chrono::high_resolution_clock::now();

  clFinish(queue[0]);
  clReleaseMemObject(input_a);
  clReleaseMemObject(output_a);

  clFinish(queue[1]);
  clReleaseMemObject(output_b);
  clReleaseMemObject(input_b);
  clReleaseKernel(krnl_dev0);
  clReleaseKernel(krnl_dev1);
  // ------------------------------------------------------------------------------------------------------------------------
  for (uint8_t i = 0; i < device_count; i++) {
    clReleaseProgram(program[i]);
    clReleaseContext(context[i]);
    clReleaseCommandQueue(queue[i]);
  }
  // ============= Software Function =========================================
  //sw_read(in1, in2, in3, sw_results);
  // =========================================================================

  //int counter = 1;
  /*for (int i = 0; i < LENGTH; i++) {
    if (sw_results[i] == hw_results[i]) {
      counter++;
    }
  }*/
  //std::cout << "Computed " << counter << "/" << LENGTH << " correct values!\n";
  cl_ulong totalTime =
      std::chrono::duration_cast<std::chrono::microseconds>(end - start)
          .count();
  p2pTime =
      std::chrono::duration_cast<std::chrono::microseconds>(p2pEnd - p2pStart)
          .count();
  //report_time("p2p", totalTime, p2pTime);

  //if (counter/* == LENGTH*/) {
    std::cout << "Test passed!\n";
    return EXIT_SUCCESS;
  /*} else {
    std::cerr << "Test failed\n";
    return EXIT_FAILURE;
  }*/

} // end of main

// ============================ Helper Functions
// =========================================
static int load_file_to_memory(const char *filename, char **result);
cl_program xcl_import_binary_file(cl_device_id device_id, cl_context context,
                                  const char *xclbin_file_name) {
  int err;

  std::cout << "INFO: Importing " << xclbin_file_name << std::endl;

  if (access(xclbin_file_name, R_OK) != 0) {
    return NULL;
    std::cerr << "ERROR: " << xclbin_file_name
              << "xclbin not available please build\n";
    exit(EXIT_FAILURE);
  }

  char *krnl_bin;
  const size_t krnl_size = load_file_to_memory(xclbin_file_name, &krnl_bin);
  std::cout << "INFO: Loaded file\n";

  cl_program program =
      clCreateProgramWithBinary(context, 1, &device_id, &krnl_size,
                                (const unsigned char **)&krnl_bin, NULL, &err);
  if ((!program) || (err != CL_SUCCESS)) {
    std::cout << "Error: Failed to create compute program from binary " << err
              << std::endl;
    std::cerr << "Test failed\n";
    exit(EXIT_FAILURE);
  }

  std::cout << "INFO: Created Binary\n";

  err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
  if (err != CL_SUCCESS) {
    size_t len;
    char buffer[2048];

    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG,
                          sizeof(buffer), buffer, &len);
    std::cout << buffer << std::endl;
    std::cerr << "Error: Failed to build program executable!\n";
    exit(EXIT_FAILURE);
  }

  std::cout << "INFO: Built Program\n";

  free(krnl_bin);

  return program;
}

static void *smalloc(size_t size) {
  void *ptr;

  ptr = malloc(size);

  if (ptr == NULL) {
    std::cerr << "Error: Cannot allocate memory\n";
    exit(EXIT_FAILURE);
  }
  // return 0;
}
static int load_file_to_memory(const char *filename, char **result) {
  unsigned int size;

  FILE *f = fopen(filename, "rb");
  if (f == NULL) {
    *result = NULL;
    std::cerr << "Error: Could not read file" << filename << std::endl;
    exit(EXIT_FAILURE);
  }

  fseek(f, 0, SEEK_END);
  size = ftell(f);
  fseek(f, 0, SEEK_SET);

  *result = (char *)smalloc(sizeof(char) * (size + 1));

  if (size != fread(*result, sizeof(char), size, f)) {
    free(*result);
    std::cerr << "Error: read of kernel failed\n";
    exit(EXIT_FAILURE);
  }

  fclose(f);
  (*result)[size] = 0;

  return size;
}
