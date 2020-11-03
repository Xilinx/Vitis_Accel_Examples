/**
* Copyright (C) 2020 Xilinx, Inc
*
* Licensed under the Apache License, Version 2.0 (the "License"). You may
* not use this file except in compliance with the License. A copy of the
* License is located at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
* License for the specific language governing permissions and limitations
* under the License.
*/
#include "cmdlineparser.h"
#include "xcl2.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>

typedef int data_t;
decltype(&xclGetMemObjectFd) xcl::P2P::getMemObjectFd = nullptr;
decltype(&xclGetMemObjectFromFd) xcl::P2P::getMemObjectFromFd = nullptr;

cl_program xcl_import_binary_file(cl_device_id device_id, cl_context context,
                                  const char *xclbin_file_name);
//==================================================================================================

/*void report_time(std::string label, cl_ulong totalTime, cl_ulong curTime) {
  double total = LENGTH;

  total *= 1000000;     // convert us to s
  total /= 1024 * 1024; // convert to MB

  std::cout << std::setw(8) << label << "\t" << std::fixed
            << std::setprecision(2) << std::setw(8) << curTime << "ms\t"
            << std::setw(8) << totalTime << "ms\t" << std::setw(8)
            << float(curTime) * 100 / totalTime << "%\t" << std::setw(8)
            << total / curTime << "MB/s\t" << std::endl;
}*/

int main(int argc, char *argv[]) {

  // Command Line Parser
  sda::utils::CmdLineParser parser;

  // Switches
  //**************//"<Full Arg>",  "<Short Arg>", "<Description>", "<Default>"
  parser.addSwitch("--xclbin_file1", "-x1", "input binary file1 string", "");
  parser.addSwitch("--xclbin_file2", "-x2", "input binary file2 string", "");
  parser.addSwitch("--device0", "-d0", "first device id", "0");
  parser.addSwitch("--device1", "-d1", "second device id", "1");
  parser.parse(argc, argv);

  // Read settings
  auto binaryFile1 = parser.value("xclbin_file1");
  auto binaryFile2 = parser.value("xclbin_file2");
  cl_uint dev_id1 = stoi(parser.value("device0"));
  cl_uint dev_id2 = stoi(parser.value("device1"));

  if (argc < 5) {
    std::cout << "Options: <exe> <-x1> <first xclbin> <-x2> <second xclbin> "
                 "<optional> <-d0> <device id0> <-d1> <device id1>"
              << endl;
    return EXIT_FAILURE;
  }

  int length = 32 * 1024 * 1024;
  size_t buffersize = 1024 * 1024;
  if (xcl::is_emulation()) {
    length = 4 * 1024;
    buffersize = 1024;
  }
  std::ofstream handle_new("data_points.txt");
  handle_new << "p2p_dev2dev\n";
  handle_new << "latency,throughput\n";

  std::vector<data_t, aligned_allocator<data_t>> in1(length);
  std::vector<data_t, aligned_allocator<data_t>> out1(length);
  std::vector<data_t, aligned_allocator<data_t>> sw_results(length);
  std::vector<data_t, aligned_allocator<data_t>> hw_results(length);
  for (int i = 0; i < length; i++) {
    in1[i] = i;
    out1[i] = 0;
    hw_results[i] = 0;
    sw_results[i] = 0;
  }

  cl_platform_id platform_id;
  cl_platform_id platforms[16] = {0};
  cl_uint platform_count;
  char platformName[256];
  cl_int error;

  clGetPlatformIDs(16, platforms, &platform_count);

  for (cl_uint i = 0; i < platform_count; i++) {
    error =
        clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 256, platformName, 0);
    if (error != CL_SUCCESS) {
      exit(EXIT_FAILURE);
    }

    if (strcmp(platformName, "Xilinx") == 0) {
      platform_id = platforms[i];
    }
  }
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

  cl_device_id device1, device2;
  if (dev_id1 <= device_count)
    device1 = device_id[dev_id1];
  else
    cout << "The device_id1 provided using -d0 flag is outside the range of "
            "available devices\n";
  if (dev_id2 <= device_count)
    device2 = device_id[dev_id2];
  else
    cout << "The device_id2 provided using -d1 flag is outside the range of "
            "available devices\n";
  cl_context context[device_count];
  cl_command_queue queue[device_count];
  cl_kernel krnl_dev0, krnl_dev1;
  cl_program program[device_count];
  int err;

  std::cout << "Initializing OpenCL objects" << std::endl;
  OCL_CHECK(err,
            context[0] = clCreateContext(0, 1, &device1, NULL, NULL, &err));
  if (err != CL_SUCCESS)
    std::cout << "clCreateContext call: Failed to create a compute context"
              << err << std::endl;
  OCL_CHECK(err, queue[0] = clCreateCommandQueue(
                     context[0], device1, CL_QUEUE_PROFILING_ENABLE, &err));
  if (err != CL_SUCCESS)
    std::cout << "clCreateCommandQueue call: Failed to create commandqueue"
              << err << std::endl;

  OCL_CHECK(err,
            context[1] = clCreateContext(0, 1, &device2, NULL, NULL, &err));
  if (err != CL_SUCCESS)
    std::cout << "clCreateContext call: Failed to create a compute context"
              << err << std::endl;
  OCL_CHECK(err, queue[1] = clCreateCommandQueue(
                     context[1], device2, CL_QUEUE_PROFILING_ENABLE, &err));
  if (err != CL_SUCCESS)
    std::cout << "clCreateCommandQueue call: Failed to create commandqueue"
              << err << std::endl;

  size_t vector_size_bytes = sizeof(data_t) * length;
  //------------------------------- Program
  //-------------------------------------------
  program[0] = xcl_import_binary_file(device1, context[0], binaryFile1.c_str());
  OCL_CHECK(err, krnl_dev0 = clCreateKernel(program[0], "bandwidth", &err));
  program[1] = xcl_import_binary_file(device2, context[1], binaryFile2.c_str());
  OCL_CHECK(err, krnl_dev1 = clCreateKernel(program[1], "bandwidth", &err));

  xcl::P2P::init(platform_id);

  // ---------------------------- Buffer-1
  // -------------------------------------------
  cl_mem input_a;
  cl_mem_ext_ptr_t inA = {0, NULL, 0};
  OCL_CHECK(err, input_a = clCreateBuffer(context[0], CL_MEM_READ_ONLY |
                                                          CL_MEM_EXT_PTR_XILINX,
                                          vector_size_bytes, &inA, &err));

  cl_mem output_a;
  cl_mem_ext_ptr_t mout = {XCL_MEM_EXT_P2P_BUFFER, NULL, 0};
  OCL_CHECK(err, output_a = clCreateBuffer(
                     context[0], CL_MEM_WRITE_ONLY | CL_MEM_EXT_PTR_XILINX,
                     vector_size_bytes, &mout, &err));

  // ---------------------------- Buffer-2
  // -------------------------------------------
  cl_mem input_b;
  cl_mem_ext_ptr_t min = {0, NULL, 0};
  OCL_CHECK(err, input_b = clCreateBuffer(context[1], CL_MEM_READ_ONLY |
                                                          CL_MEM_EXT_PTR_XILINX,
                                          vector_size_bytes, &min, &err));

  cl_mem output_b;
  cl_mem_ext_ptr_t ot = {XCL_MEM_EXT_P2P_BUFFER, NULL, 0};
  OCL_CHECK(err, output_b = clCreateBuffer(
                     context[1], CL_MEM_WRITE_ONLY | CL_MEM_EXT_PTR_XILINX,
                     vector_size_bytes, &ot, &err));

  // ----------------------------Set Args
  // -------------------------------------------
  std::cout << "Set Args FPGA-1" << std::endl;
  OCL_CHECK(err, err = clSetKernelArg(krnl_dev0, 0, sizeof(cl_mem), &input_a));
  OCL_CHECK(err, err = clSetKernelArg(krnl_dev0, 1, sizeof(cl_mem), &output_a));

  std::cout << "Set Args FPGA-2\n" << std::endl;
  OCL_CHECK(err, err = clSetKernelArg(krnl_dev1, 0, sizeof(cl_mem), &input_b));
  OCL_CHECK(err, err = clSetKernelArg(krnl_dev1, 1, sizeof(cl_mem), &output_b));

  // -----------------------------------------------------------------------
  std::cout << "Write data to FPGA-1" << std::endl;
  OCL_CHECK(err, err = clEnqueueWriteBuffer(queue[0], input_a, CL_TRUE, 0,
                                            vector_size_bytes, in1.data(), 0,
                                            NULL, NULL));

  std::cout << "Launch FPGA-1" << std::endl;
  OCL_CHECK(err, err = clEnqueueTask(queue[0], krnl_dev0, 0, NULL, NULL));
  clFinish(queue[0]);

  //------------------------- P2P
  //-----------------------------------------------------------
  std::cout << "Transferring from FPGA-1 to FPGA-2..." << std::endl;
  int fd = -1;
  OCL_CHECK(err,
            err = xcl::P2P::getMemObjectFd(
                output_b, &fd)); // Export p2p buffer to file descriptor (fd)
  if (fd > 0) {
    printf("Export FD:%d\n", fd);
  }

  cl_mem exported_buf;
  OCL_CHECK(err, err = xcl::P2P::getMemObjectFromFd(context[0], device1, 0, fd,
                                                    &exported_buf)); // Import
  cl_event event;

  for (size_t bufsize = buffersize; bufsize <= vector_size_bytes;
       bufsize *= 2) {
    std::cout << "Now start P2P copy " << bufsize
              << " Bytes from a device to another device" << std::endl;
    int burst = vector_size_bytes / bufsize;
    std::chrono::high_resolution_clock::time_point p2pStart =
        std::chrono::high_resolution_clock::now();
    int iter = 1;
    for (int j = 0; j < iter; j++) {
      for (int i = 0; i < burst; i++) {
        OCL_CHECK(err, err = clEnqueueCopyBuffer(queue[0], input_a,
                                                 exported_buf, 0, 0, bufsize, 0,
                                                 NULL, &event)); // transfer
        clWaitForEvents(1, &event);
      }
    }
    std::chrono::high_resolution_clock::time_point p2pEnd =
        std::chrono::high_resolution_clock::now();
    cl_ulong p2pTime =
        std::chrono::duration_cast<std::chrono::microseconds>(p2pEnd - p2pStart)
            .count();
    ;
    double dnsduration = (double)p2pTime;
    double dsduration = dnsduration / ((double)1000000);
    double gbpersec =
        (vector_size_bytes * iter / dsduration) / ((double)1024 * 1024 * 1024);
    handle_new << "{\"metric\": \"copy_latency\", \"buf_size_bytes\": "
               << bufsize << ", \"number_of_bufs\": " << burst
               << ", \"per_buffer_latency_sec\": " << dsduration / burst
               << "}\n";
    handle_new << "{\"metric\": \"copy_throughput\", \"buf_size_bytes\": "
               << bufsize << ", \"number_of_bufs\": " << burst
               << ", \"throughput_gb_per_sec\": " << gbpersec << "}\n";
    std::cout << "{\"metric\": \"copy_latency\", \"buf_size_bytes\": "
              << bufsize << ", \"number_of_bufs\": " << burst
              << ", \"per_buffer_latency_sec\": " << dsduration / burst
              << "}\n";
    std::cout << "{\"metric\": \"copy_throughput\", \"buf_size_bytes\": "
              << bufsize << ", \"number_of_bufs\": " << burst
              << ", \"throughput_gb_per_sec\": " << gbpersec << "}\n";
  }

  handle_new.close();
  clFinish(queue[0]);
  clReleaseMemObject(input_a);
  clReleaseMemObject(output_a);
  clReleaseMemObject(exported_buf);

  clFinish(queue[1]);
  clReleaseMemObject(input_b);
  clReleaseMemObject(output_b);
  clReleaseKernel(krnl_dev0);
  clReleaseKernel(krnl_dev1);
  // ------------------------------------------------------------------------------------------------------------------------
  for (uint i = 0; i < device_count; i++) {
    clReleaseProgram(program[i]);
    clReleaseContext(context[i]);
    clReleaseCommandQueue(queue[i]);
  }

  std::cout << "Test passed!\n";
  return EXIT_SUCCESS;

} // end of main

// ============================ Helper Functions
// =========================================
static int load_file_to_memory(const char *filename, char **result);
cl_program xcl_import_binary_file(cl_device_id device_id, cl_context context,
                                  const char *xclbin_file_name) {
  int err;

  printf("INFO: Importing %s\n", xclbin_file_name);

  if (access(xclbin_file_name, R_OK) != 0) {
    return NULL;
    printf("ERROR: %s xclbin not available please build\n", xclbin_file_name);
    exit(EXIT_FAILURE);
  }

  char *krnl_bin;
  const size_t krnl_size = load_file_to_memory(xclbin_file_name, &krnl_bin);
  printf("INFO: Loaded file\n");

  cl_program program =
      clCreateProgramWithBinary(context, 1, &device_id, &krnl_size,
                                (const unsigned char **)&krnl_bin, NULL, &err);
  if ((!program) || (err != CL_SUCCESS)) {
    printf("Error: Failed to create compute program from binary %d!\n", err);
    printf("Test failed\n");
    exit(EXIT_FAILURE);
  }
  printf("INFO: Created Binary\n");

  err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
  if (err != CL_SUCCESS) {
    size_t len;
    char buffer[2048];

    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG,
                          sizeof(buffer), buffer, &len);
    printf("%s\n", buffer);
    printf("Error: Failed to build program executable!\n");
    exit(EXIT_FAILURE);
  }

  printf("INFO: Built Program\n");

  free(krnl_bin);
  return program;
}

static void *smalloc(size_t size);
static int load_file_to_memory(const char *filename, char **result) {
  unsigned int size;

  FILE *f = fopen(filename, "rb");
  if (f == NULL) {
    *result = NULL;
    printf("Error: Could not read file %s\n", filename);
    exit(EXIT_FAILURE);
  }

  fseek(f, 0, SEEK_END);
  size = ftell(f);
  fseek(f, 0, SEEK_SET);

  *result = (char *)smalloc(sizeof(char) * (size + 1));

  if (size != fread(*result, sizeof(char), size, f)) {
    free(*result);
    printf("Error: read of kernel failed\n");
    exit(EXIT_FAILURE);
  }

  fclose(f);
  (*result)[size] = 0;

  return size;
}

static void *smalloc(size_t size) {
  void *ptr;

  ptr = malloc(size);

  if (ptr == NULL) {
    printf("Error: Cannot allocate memory\n");
    exit(EXIT_FAILURE);
  }
  return ptr;
}
