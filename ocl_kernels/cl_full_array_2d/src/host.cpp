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
#include "xcl2.hpp"
#include <vector>

// Define array size
#define TEST_MATRIX_DIM 256

// Define max local buffer size
#define MAX_MATRIX_DIM 256

void mmult_sw(std::vector<int, aligned_allocator<int>> &a,
              std::vector<int, aligned_allocator<int>> &b,
              std::vector<int, aligned_allocator<int>> &c,
              int size) {
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            int result = 0;
            for (int k = 0; k < size; k++) {
                result += a[row * size + k] * b[k * size + col];
            }
            c[row * size + col] = result;
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];

    int test_matrix_dim = TEST_MATRIX_DIM;
    const char *xcl_emu = getenv("XCL_EMULATION_MODE");
    if (xcl_emu && !strcmp(xcl_emu, "hw_emu")) {
        test_matrix_dim = 16;
        std::cout << "Data Size Reduced to  " << test_matrix_dim
                  << " for Hardware Emulation" << std::endl;
    }

    int dim = test_matrix_dim;
    int matrix_size = dim * dim;

    if (dim > MAX_MATRIX_DIM) {
        std::cout << "Size is bigger than internal buffer size, please use a "
                     "size smaller than 256!"
                  << std::endl;
        return EXIT_FAILURE;
    }
    //Allocate Memory in Host Memory
    size_t matrix_size_in_bytes = sizeof(int) * matrix_size;
    std::vector<int, aligned_allocator<int>> source_input_a(matrix_size);
    std::vector<int, aligned_allocator<int>> source_input_b(matrix_size);
    std::vector<int, aligned_allocator<int>> source_hw_results(matrix_size);
    std::vector<int, aligned_allocator<int>> source_sw_results(matrix_size);
    cl_int err;
    unsigned fileBufSize;

    // Create the test data and Software Result
    for (int i = 0; i < matrix_size; i++) {
        source_input_a[i] = i;
        source_input_b[i] = i;
        source_hw_results[i] = 0;
    }
    mmult_sw(source_input_a, source_input_b, source_sw_results, dim);

    //OPENCL HOST CODE AREA START
    auto devices = xcl::get_xil_devices();
    auto device = devices[0];

    OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(
        err,
        cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
    OCL_CHECK(err,
              std::string device_name = device.getInfo<CL_DEVICE_NAME>(&err));

    auto fileBuf = xcl::read_binary_file(binaryFile, fileBufSize);
    cl::Program::Binaries bins{{fileBuf, fileBufSize}};
    devices.resize(1);
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
    OCL_CHECK(err, cl::Kernel krnl_mmult(program, "mmult", &err));

    //Allocate Buffer in Global Memory
    OCL_CHECK(err,
              cl::Buffer buffer_a(context,
                                  CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                  matrix_size_in_bytes,
                                  source_input_a.data(),
                                  &err));
    OCL_CHECK(err,
              cl::Buffer buffer_b(context,
                                  CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                  matrix_size_in_bytes,
                                  source_input_b.data(),
                                  &err));
    OCL_CHECK(err,
              cl::Buffer buffer_c(context,
                                  CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                                  matrix_size_in_bytes,
                                  source_hw_results.data(),
                                  &err));

    OCL_CHECK(err, err = krnl_mmult.setArg(0, buffer_a));
    OCL_CHECK(err, err = krnl_mmult.setArg(1, buffer_b));
    OCL_CHECK(err, err = krnl_mmult.setArg(2, buffer_c));
    OCL_CHECK(err, err = krnl_mmult.setArg(3, dim));

    //Migrate  input data to device global memory
    OCL_CHECK(err,
              err = q.enqueueMigrateMemObjects({buffer_a, buffer_b},
                                               0 /* 0 means from host*/));

    //Launch the Kernel
    OCL_CHECK(err, err = q.enqueueTask(krnl_mmult));

    //Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err,
              err = q.enqueueMigrateMemObjects({buffer_c},
                                               CL_MIGRATE_MEM_OBJECT_HOST));
    OCL_CHECK(err, err = q.finish());
    //OPENCL HOST CODE AREA END

    // Compare the results of the Device to the simulation
    int match = 0;
    for (int i = 0; i < matrix_size; i++) {
        if (source_hw_results[i] != source_sw_results[i]) {
            std::cout << "Error: Result mismatch" << std::endl;
            std::cout << "i = " << i << " CPU result = " << source_sw_results[i]
                      << " Device result = " << source_hw_results[i]
                      << std::endl;
            match = 1;
            break;
        }
    }

    delete[] fileBuf;

    std::cout << "TEST " << (match ? "FAILED" : "PASSED") << std::endl;
    return (match ? EXIT_FAILURE : EXIT_SUCCESS);
}
