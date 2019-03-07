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
**********/

//OpenCL utility layer include
#include "xcl2.hpp"
#include <vector>
#include <algorithm>
#include <cstdio>
#include <random>

using std::default_random_engine;
using std::generate;
using std::uniform_int_distribution;
using std::vector;

void matmul(int *C, int *A, int *B, int M) {
    for (int k = 0; k < M; k++) {
        for (int j = 0; j < M; j++) {
            for (int i = 0; i < M; i++) {
                C[k * M + j] += A[k * M + i] * B[i * M + j];
            }
        }
    }
}

int gen_random() {
  static default_random_engine e;
  static uniform_int_distribution<int> dist(0, 10);

  return dist(e);
}


void print(int *data, int columns, int rows) {
    vector<int> out(columns * rows);
    for (int r = 0; r < 10; r++) {
        for (int c = 0; c < 10; c++) {
            printf("%4d ", data[r * columns + c]);
        }
        printf("…\n");
    }
    for (int r = 0; r < 10; r++) {
      printf("   %s ", "…");
    }
    printf("⋱\n\n");
}

void verify(vector<int,aligned_allocator<int>> &gold, 
            vector<int,aligned_allocator<int>> &output) {
    for (int i = 0; i < (int)output.size(); i++) {
        if (output[i] != gold[i]) {
            printf("Mismatch %d: gold: %d device: %d\n", i, gold[i], output[i]);
            print(output.data(), 16, 16);
            exit(EXIT_FAILURE);
        }
    }
}

// This example illustrates how to use array partitioning attributes in HLS
// kernels for FPGA devices using matmul.
int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
		return EXIT_FAILURE;
	}
    std::string binaryFile = argv[1];
    static const int columns = 16;
    static const int rows = 16;
    cl_int err;
    unsigned fileBufSize;
    
    vector<int,aligned_allocator<int>> A(columns * rows);
    vector<int,aligned_allocator<int>> B(columns * rows);
    vector<int,aligned_allocator<int>> gold(columns * rows, 0);
    vector<int,aligned_allocator<int>> C(columns * rows, 0);
    generate(begin(A), end(A), gen_random);
    generate(begin(B), end(B), gen_random);

    printf("A:\n");
    print(A.data(), columns, rows);
    printf("B:\n");
    print(B.data(), columns, rows);
    matmul(gold.data(), A.data(), B.data(), columns);

    printf("Gold:\n");
    print(gold.data(), columns, rows);
    std::vector<cl::Device> devices = xcl::get_xil_devices();
    cl::Device device = devices[0];

    OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(err, cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
    OCL_CHECK(err, std::string device_name = device.getInfo<CL_DEVICE_NAME>(&err));

    //Create Program 
    char* fileBuf = xcl::read_binary_file(binaryFile, fileBufSize);
    cl::Program::Binaries bins{{fileBuf, fileBufSize}};
    devices.resize(1);
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
    
    // compute the size of array in bytes
    size_t array_size_bytes = columns * rows * sizeof(int);
    OCL_CHECK(err, cl::Buffer buffer_a(context,CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, 
            array_size_bytes, A.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_b(context,CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, 
            array_size_bytes, B.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_c(context,CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
            array_size_bytes, C.data(), &err));

  printf( "|-------------------------+-------------------------|\n"
          "| Kernel                  |    Wall-Clock Time (ns) |\n"
          "|-------------------------+-------------------------|\n");
    
    OCL_CHECK(err, cl::Kernel matmul_kernel(program, "matmul", &err));
    OCL_CHECK(err, err = matmul_kernel.setArg(0,buffer_a));
    OCL_CHECK(err, err = matmul_kernel.setArg(1,buffer_b));
    OCL_CHECK(err, err = matmul_kernel.setArg(2,buffer_c));
    OCL_CHECK(err, err = matmul_kernel.setArg(3,columns));

    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_a, buffer_b},0/* 0 means from host*/));

    cl::Event event;
    uint64_t nstimestart, nstimeend;
    
    OCL_CHECK(err, err = q.enqueueTask(matmul_kernel,NULL,&event));
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_c},CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();
    
    OCL_CHECK(err, err = event.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_START,&nstimestart));
    OCL_CHECK(err, err = event.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_END,&nstimeend));
    auto matmul_time = nstimeend-nstimestart;

    verify(gold, C);
    printf("| %-23s | %23lu |\n", "matmul: ", matmul_time);

    OCL_CHECK(err, cl::Kernel matmul_partition_kernel(program, "matmul_partition", &err));
    
    OCL_CHECK(err, err = matmul_partition_kernel.setArg(0,buffer_a));
    OCL_CHECK(err, err = matmul_partition_kernel.setArg(1,buffer_b));
    OCL_CHECK(err, err = matmul_partition_kernel.setArg(2,buffer_c));
    OCL_CHECK(err, err = matmul_partition_kernel.setArg(3,columns));

    OCL_CHECK(err, err = q.enqueueTask(matmul_partition_kernel,NULL,&event));
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_c},CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();
    
    OCL_CHECK(err, err = event.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_START,&nstimestart));
    OCL_CHECK(err, err = event.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_END,&nstimeend));
    auto matmul_partition_time = nstimeend-nstimestart;

    verify(gold, C);

    delete[] fileBuf;

    printf("| %-23s | %23lu |\n", "matmul: partition", matmul_partition_time);

    printf("|-------------------------+-------------------------|\n");
    printf("Note: Wall Clock Time is meaningful for real hardware execution only, not for emulation.\n");
    printf("Please refer to profile summary for kernel execution time for hardware emulation.\n");
    printf("TEST PASSED\n\n");

    return EXIT_SUCCESS;
}
