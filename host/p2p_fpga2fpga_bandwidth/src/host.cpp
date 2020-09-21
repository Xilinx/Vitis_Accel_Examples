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
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <CL/opencl.h>
#include <vector>
#include "xcl2.hpp"

#define INCR_VALUE 10
typedef int data_t; 
decltype(&xclGetMemObjectFd) xcl::P2P::getMemObjectFd = nullptr;
decltype(&xclGetMemObjectFromFd) xcl::P2P::getMemObjectFromFd = nullptr;

cl_program xcl_import_binary_file(cl_device_id device_id, cl_context context, const char *xclbin_file_name);
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

int main(int argc, char* argv[]) {
    int length = 128*1024*1024;//std::stoi(argv[3]);//length;
    size_t buffersize=1024*1024;
    if (xcl::is_emulation()) {
        length = 4 * 1024;
        buffersize = 4;
    }
    int inc = INCR_VALUE;
    std::ofstream handle_new("data_points.txt");
    handle_new << "p2p_dev2dev\n";
    handle_new << "latency,throughput\n";
    //
    
    //data_t in1[length];              // original data set given to device
    //data_t out1[length];
    //data_t hw_results[length];    
    //data_t sw_results[length];       
    std::vector<data_t, aligned_allocator<data_t>> in1(length);
    std::vector<data_t, aligned_allocator<data_t>> out1(length);
    std::vector<data_t, aligned_allocator<data_t>> sw_results(length);
    std::vector<data_t, aligned_allocator<data_t>> hw_results(length);
    for(int i = 0; i < length; i++) {
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
       error = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 256, platformName, 0);
       if (error != CL_SUCCESS) {
          exit(EXIT_FAILURE);
       }

       if(strcmp(platformName, "Xilinx")==0)
       {
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

    cl_context context[device_count];
    cl_command_queue queue[device_count];
    cl_kernel krnl_dev0, krnl_dev1;
    cl_program program[device_count];
    int err;

    std::cout <<"Initializing OpenCL objects" << std::endl;
    for(uint i=0; i<device_count; i++) {
        OCL_CHECK(err, context[i] = clCreateContext(0, 1, &device_id[i], NULL, NULL, &err));
        OCL_CHECK(err, queue[i] = clCreateCommandQueue(context[i], device_id[i], CL_QUEUE_PROFILING_ENABLE, &err));

  /*std::cout << "Initializing OpenCL objects" << std::endl;
  for (uint8_t i = 0; i < device_count; i++) {
    context[i] = clCreateContext(0, 1, &device_id[i], NULL, NULL, &err);
    if (err != CL_SUCCESS)
      std::cout << "clCreateContext call: Failed to create a compute context"
                << err << std::endl;
    queue[i] = clCreateCommandQueue(context[i], device_id[i],
                                    CL_QUEUE_PROFILING_ENABLE, &err);
    if (err != CL_SUCCESS)
      std::cout << "clCreateCommandQueue call: Failed to create commandqueue"
                << err << std::endl;*/
    }
    
    size_t vector_size_bytes = sizeof(data_t)*length;
    size_t global[1];
    size_t local[1];
    //------------------------------- Program ------------------------------------------- 
    program[0] = xcl_import_binary_file(device_id[0], context[0], argv[1]);
    OCL_CHECK(err, krnl_dev0 = clCreateKernel(program[0], "bandwidth", &err));
    program[1] = xcl_import_binary_file(device_id[1], context[1], argv[1]);
    OCL_CHECK(err, krnl_dev1 = clCreateKernel(program[1], "bandwidth", &err));

    xcl::P2P::init(platform_id);

    // ---------------------------- Buffer-1 -------------------------------------------
    cl_mem input_a;                       
	cl_mem_ext_ptr_t inA = {0, NULL, 0};
	OCL_CHECK( err, input_a = clCreateBuffer(context[0], CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, vector_size_bytes, &inA, &err));
	
    cl_mem output_a;                       
	cl_mem_ext_ptr_t mout = {XCL_MEM_EXT_P2P_BUFFER, NULL, 0};
	OCL_CHECK( err, output_a = clCreateBuffer(context[0], CL_MEM_WRITE_ONLY | CL_MEM_EXT_PTR_XILINX, vector_size_bytes, &mout, &err));
    
    // ---------------------------- Buffer-2 -------------------------------------------
    cl_mem input_b;                       
	cl_mem_ext_ptr_t min = {0, NULL, 0};
	OCL_CHECK( err, input_b = clCreateBuffer(context[1], CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, vector_size_bytes, &min, &err));
   
    cl_mem output_b;                       
	cl_mem_ext_ptr_t ot = {XCL_MEM_EXT_P2P_BUFFER, NULL, 0};
	OCL_CHECK( err, output_b = clCreateBuffer(context[1], CL_MEM_WRITE_ONLY | CL_MEM_EXT_PTR_XILINX, vector_size_bytes, &ot, &err));
  
    // ----------------------------Set Args -------------------------------------------
    std::cout << "Set Args FPGA-1" << std::endl;
    OCL_CHECK(err,
              err = clSetKernelArg(krnl_dev0, 0, sizeof(cl_mem), &input_a));
    OCL_CHECK(err,
              err = clSetKernelArg(krnl_dev0, 1, sizeof(cl_mem), &output_a));

    std::cout << "Set Args FPGA-2\n" << std::endl;
    OCL_CHECK(err,
            err = clSetKernelArg(krnl_dev1, 0, sizeof(cl_mem), &input_b));
    OCL_CHECK(err,
            err = clSetKernelArg(krnl_dev1, 1, sizeof(cl_mem), &output_b));

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    // -----------------------------------------------------------------------
    std::cout << "Write data to FPGA-1" << std::endl;
    OCL_CHECK (err, err = clEnqueueWriteBuffer(queue[0], input_a, CL_TRUE, 0, vector_size_bytes, in1.data(), 0, NULL, NULL));
    
    std::cout << "Launch FPGA-1" << std::endl;
    OCL_CHECK(err, err = clEnqueueTask(queue[0], krnl_dev0, 0, NULL, NULL)); 
    clFinish(queue[0]);

    //------------------------- P2P -----------------------------------------------------------
    std::cout << "Transferring from FPGA-1 to FPGA-2..." << std::endl;
    int fd=-1;
    OCL_CHECK(err, err = xcl::P2P::getMemObjectFd(output_b, &fd));  // Export p2p buffer to file descriptor (fd)
    if(fd>0) {
        printf("Export FD:%d\n", fd);
    }

    cl_mem exported_buf;
    OCL_CHECK(err, err = xcl::P2P::getMemObjectFromFd(context[0], device_id[0], 0, fd, &exported_buf)); // Import 
    cl_event event;


  for(size_t bufsize=buffersize; bufsize<=vector_size_bytes; bufsize*=2){
    std::cout << "Now start P2P copy "<< bufsize<< " Bytes from a device to another device" << std::endl;
    int burst = vector_size_bytes / bufsize;
    std::chrono::high_resolution_clock::time_point p2pStart = std::chrono::high_resolution_clock::now();
    int iter = 1;
    for (int j = 0; j < iter ; j++){
    for(int i=0; i<burst; i++) {
        OCL_CHECK(err, err = clEnqueueCopyBuffer(queue[0], input_a, exported_buf, 0, 0, bufsize, 0, NULL, &event)); //transfer
        clWaitForEvents(1, &event);
    }
    }
    std::chrono::high_resolution_clock::time_point p2pEnd = std::chrono::high_resolution_clock::now();
    cl_ulong p2pTime = std::chrono::duration_cast<std::chrono::microseconds>(p2pEnd - p2pStart).count();;
    double dnsduration = (double)p2pTime;
    double dsduration = dnsduration / ((double)1000000);
    double gbpersec = (vector_size_bytes *iter / dsduration) / ((double)1024 * 1024 * 1024);
    handle_new << "{\"metric\": \"copy_latency\", \"buf_size_bytes\": " << bufsize << ", \"number_of_bufs\": " << burst << ", \"per_buffer_latency_sec\": " << dsduration / burst << "}\n";
    handle_new << "{\"metric\": \"copy_throughput\", \"buf_size_bytes\": " << bufsize << ", \"number_of_bufs\": " << burst << ", \"throughput_gb_per_sec\": " << gbpersec << "}\n";
    std::cout << "{\"metric\": \"copy_latency\", \"buf_size_bytes\": " << bufsize << ", \"number_of_bufs\": " << burst << ", \"per_buffer_latency_sec\": " << dsduration / burst << "}\n";
    std::cout << "{\"metric\": \"copy_throughput\", \"buf_size_bytes\": " << bufsize << ", \"number_of_bufs\": " << burst << ", \"throughput_gb_per_sec\": " << gbpersec << "}\n";
  }
    
    // -----------------------------------------------------------------------
    /*std::cout << "Write data to FPGA-2" << std::endl;
    
    std::cout << "Launch FPGA-2" << std::endl;
    OCL_CHECK(err, err = clEnqueueTask(queue[1], kernel[1], 0, NULL, NULL));  

    std::cout << "Read data back from FPGA-2" << std::endl;
    OCL_CHECK (err, err = clEnqueueReadBuffer(queue[1], out, CL_TRUE, 0, vector_size_bytes, hw_results.data(), 0, NULL, NULL));*/
    
    handle_new.close();
    //clEnqueueUnmapMemObject(queue[0], output_a, (void*)out1, 0, NULL, NULL);
    //std::cout << "Unmapped" << std::endl;
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
    for(uint i=0; i<device_count; i++) {
        clReleaseProgram(program[i]);
        clReleaseContext(context[i]);
        clReleaseCommandQueue(queue[i]);
    }

  std::cout << "Test passed!\n";
  return EXIT_SUCCESS;

} // end of main

// ============================ Helper Functions =========================================
static int load_file_to_memory(const char *filename, char **result);
cl_program xcl_import_binary_file(cl_device_id device_id, cl_context context, const char *xclbin_file_name) {
	int err;

	printf("INFO: Importing %s\n", xclbin_file_name);

	if(access(xclbin_file_name, R_OK) != 0) {
		return NULL;
		printf("ERROR: %s xclbin not available please build\n", xclbin_file_name);
		exit(EXIT_FAILURE);
	}

	char *krnl_bin;
	const size_t krnl_size = load_file_to_memory(xclbin_file_name, &krnl_bin);
	printf("INFO: Loaded file\n");

	cl_program program = clCreateProgramWithBinary(context, 1,
	                                    &device_id, &krnl_size,
	                                    (const unsigned char**) &krnl_bin,
	                                    NULL, &err);
	if ((!program) || (err!=CL_SUCCESS)) {
		printf("Error: Failed to create compute program from binary %d!\n",
		       err);
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

static void* smalloc(size_t size);
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

	*result = (char *) smalloc(sizeof(char)*(size+1));

	if (size != fread(*result, sizeof(char), size, f)) {
		free(*result);
		printf("Error: read of kernel failed\n");
		exit(EXIT_FAILURE);
	}

	fclose(f);
	(*result)[size] = 0;

	return size;
}

static void* smalloc(size_t size) {
	void* ptr;

	ptr = malloc(size);

	if (ptr == NULL) {
		printf("Error: Cannot allocate memory\n");
		exit(EXIT_FAILURE);
	}
}

