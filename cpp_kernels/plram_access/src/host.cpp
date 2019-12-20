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

/*****
This example demonstrates how PLRAM feature of the Vitis memory subsystem and how they 
integrate with the Vitis design process.
PLRAMs are small shared memories which are built using the on-chip memory resources 
of the FPGA fabric. They are intended to provide a small amount of data storage that
application kernels can share and access rapidly. PLRAMs behave just like the DDR 
memory resources managed by the Vitis memory subsystem.
*****/

//OpenCL utility layer include
#include "xcl2.hpp"
#include <algorithm>
#include <stdlib.h>
#include <vector>
//Array Size to access
#define DATA_SIZE 8

//Binary File string
std::string binaryFile;

//CPU implementation of Matrix Multiplication
//The inputs are of the size (DATA_SIZE x DATA_SIZE)
void mmult_cpu(int *in1, //Input Matrix 1
               int *in2, //Input Matrix 1
               int *out, //Input Matrix 1
               int dim   //One dimension of matrix
) {
    //Performs Matrix multiply Out = In1 x In2
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
            for (int k = 0; k < dim; k++) {
                out[i * dim + j] += in1[i * dim + k] * in2[k * dim + j];
            }
        }
    }
}

//Functionality to setup OpenCL context and trigger the Kernel
void mmult_fpga(
    std::vector<int, aligned_allocator<int>> &source_in1, //Input Matrix 1
    std::vector<int, aligned_allocator<int>> &source_in2, //Input Matrix 2
    std::vector<int, aligned_allocator<int>>
        &source_fpga_results, //Output Matrix
    int dim                   //One dimension of matrix
) {
    int size = dim;
    size_t matrix_size_bytes = sizeof(int) * size * size;
    cl::CommandQueue q;
    cl::Context context;
    cl::Kernel kernel;
    cl_int err;
    //The get_xil_devices will return vector of Xilinx Devices
    auto devices = xcl::get_xil_devices();
    // read_binary_file() is a utility API which will load the binaryFile
    // and will return the pointer to file buffer.
    auto fileBuf = xcl::read_binary_file(binaryFile);
    cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    int valid_device = 0;
    for (unsigned int i = 0; i < devices.size(); i++) {
        auto device = devices[i];
        // Creating Context and Command Queue for selected Device
        OCL_CHECK(err, context = cl::Context({device}, NULL, NULL, NULL, &err));
        OCL_CHECK(err,
                  q = cl::CommandQueue(
                      context, {device}, CL_QUEUE_PROFILING_ENABLE, &err));

        std::cout << "Trying to program device[" << i
                  << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
        cl::Program program(context, {device}, bins, NULL, &err);
        if (err != CL_SUCCESS) {
            std::cout << "Failed to program device[" << i
                      << "] with xclbin file!\n";
        } else {
            std::cout << "Device[" << i << "]: program successful!\n";
            OCL_CHECK(err, kernel = cl::Kernel(program, "mmult", &err));
            valid_device++;
            break; // we break because we found a valid device
        }
    }
    if (valid_device == 0) {
        std::cout << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }

    cl::Buffer buffer_in1(context,
                          CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                          matrix_size_bytes,
                          source_in1.data(),
                          &err);

    cl::Buffer buffer_in2(context,
                          CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                          matrix_size_bytes,
                          source_in2.data());

    cl::Buffer buffer_output(context,
                             CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                             matrix_size_bytes,
                             source_fpga_results.data(),
                             &err);

    /* 
     * Using setArg(), i.e. setting kernel arguments, explicitly before enqueueMigrateMemObjects(), 
     * i.e. copying host memory to device memory,  allowing runtime to associate buffer with correct
     * DDR banks automatically. 
    */

    int a_row = DATA_SIZE;
    int a_col = DATA_SIZE;
    int b_col = DATA_SIZE;
    //Set the kernel arguments
    int narg = 0;
    kernel.setArg(narg++, buffer_in1);
    kernel.setArg(narg++, buffer_in2);
    kernel.setArg(narg++, buffer_output);
    kernel.setArg(narg++, a_row);
    kernel.setArg(narg++, a_col);
    kernel.setArg(narg++, b_col);

    q.enqueueMigrateMemObjects({buffer_in1, buffer_in2},
                               0 /* 0 means from host*/);

    //Launch the kernel
    q.enqueueTask(kernel);

    q.enqueueMigrateMemObjects({buffer_output}, CL_MIGRATE_MEM_OBJECT_HOST);
    q.finish();
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    binaryFile = argv[1];

    //Allocate Memory in Host Memory
    int size = DATA_SIZE;
    size_t matrix_size_bytes = sizeof(int) * size * size;

    std::vector<int, aligned_allocator<int>> source_in1(matrix_size_bytes);
    std::vector<int, aligned_allocator<int>> source_in2(matrix_size_bytes);
    std::vector<int, aligned_allocator<int>> source_fpga_results(
        matrix_size_bytes);
    std::vector<int, aligned_allocator<int>> source_cpu_results(
        matrix_size_bytes);

    //Create the test data
    std::generate(source_in1.begin(), source_in1.end(), std::rand);

    std::generate(source_in2.begin(), source_in2.end(), std::rand);
    for (int i = 0; i < DATA_SIZE * DATA_SIZE; i++) {
        source_cpu_results[i] = 0;
        source_fpga_results[i] = 0;
    }

    //Compute CPU Results
    mmult_cpu(
        source_in1.data(), source_in2.data(), source_cpu_results.data(), size);

    //Compute FPGA Results
    mmult_fpga(source_in1, source_in2, source_fpga_results, size);

    //Compare the results of FPGA to CPU
    bool match = true;
    for (int i = 0; i < size * size; i++) {
        if (source_fpga_results[i] != source_cpu_results[i]) {
            std::cout << "Error: Result mismatch" << std::endl;
            std::cout << "i = " << i
                      << " CPU result = " << source_cpu_results[i]
                      << " FPGA result = " << source_fpga_results[i]
                      << std::endl;
            match = false;
            break;
        }
    }

    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;

    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
