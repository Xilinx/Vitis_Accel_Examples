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

/*****
This example demonstrates how PLRAM feature of the SDx memory subsystem and how they 
integrate with the SDx design process.
PLRAMs are small shared memories which are built using the on-chip memory resources 
of the FPGA fabric. They are intended to provide a small amount of data storage that
application kernels can share and access rapidly. PLRAMs behave just like the DDR 
memory resources managed by the SDx memory subsystem.
*****/

//OpenCL utility layer include
#include "xcl2.hpp"
#include <stdlib.h> 
#include <vector>

//Array Size to access 
#define DATA_SIZE 8

//Binary File string
std::string binaryFile;

//CPU implementation of Matrix Multiplication
//The inputs are of the size (DATA_SIZE x DATA_SIZE)
void mmult_cpu (
    int *in1,   //Input Matrix 1
    int *in2,   //Input Matrix 1
    int *out,   //Input Matrix 1
    int dim     //One dimension of matrix
)
{
    //Performs Matrix multiply Out = In1 x In2
    for(int i = 0; i < dim; i++) {
        for(int j = 0; j < dim; j++) {
            for(int k = 0; k < dim; k++) {
                out[i * dim + j] += in1[i * dim + k] * in2[k * dim + j];
            }
        }
    }
}  

//Functionality to setup OpenCL context and trigger the Kernel
void mmult_fpga (
    std::vector<int,aligned_allocator<int>>& source_in1,   //Input Matrix 1
    std::vector<int,aligned_allocator<int>>& source_in2,   //Input Matrix 2
    std::vector<int,aligned_allocator<int>>& source_fpga_results,    //Output Matrix
    int dim                                                //One dimension of matrix
)
{
    int size = dim;    
    size_t matrix_size_bytes = sizeof(int) * size * size;

    unsigned fileBufSize;
    //The get_xil_devices will return vector of Xilinx Devices 
    std::vector<cl::Device> devices = xcl::get_xil_devices();
    cl::Device device = devices[0];

    //Creating Context and Command Queue for selected Device
    cl::Context context(device);
    cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE);
    std::string device_name = device.getInfo<CL_DEVICE_NAME>(); 

    char* fileBuf = xcl::read_binary_file(binaryFile, fileBufSize);
    cl::Program::Binaries bins{{fileBuf, fileBufSize}};
    devices.resize(1);
    cl::Program program(context, devices, bins);

    cl::Kernel kernel(program,"mmult");

    cl_int err;
    cl::Buffer buffer_in1(context,CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        matrix_size_bytes,source_in1.data(),&err);
    
    cl::Buffer buffer_in2(context,CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
        matrix_size_bytes,source_in2.data());
    
    cl::Buffer buffer_output(context,CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
        matrix_size_bytes,source_fpga_results.data(),&err);

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

    q.enqueueMigrateMemObjects({buffer_in1, buffer_in2},0/* 0 means from host*/);
    
    //Launch the kernel
    q.enqueueTask(kernel);

    q.enqueueMigrateMemObjects({buffer_output},CL_MIGRATE_MEM_OBJECT_HOST);
    q.finish();

    delete[] fileBuf;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    binaryFile = argv[1];

    //Allocate Memory in Host Memory
    int size = DATA_SIZE;    
    size_t matrix_size_bytes = sizeof(int) * size * size;

    std::vector<int,aligned_allocator<int>> source_in1(matrix_size_bytes);
    std::vector<int,aligned_allocator<int>> source_in2(matrix_size_bytes);
    std::vector<int,aligned_allocator<int>> source_fpga_results(matrix_size_bytes);
    std::vector<int,aligned_allocator<int>> source_cpu_results(matrix_size_bytes);

    //Create the test data 
    for(int i = 0 ; i < DATA_SIZE * DATA_SIZE ; i++){
        source_in1[i] = rand() % size;
        source_in2[i] = rand() % size;
        source_cpu_results[i] = 0;
        source_fpga_results[i] = 0;
    }

    //Compute CPU Results
    mmult_cpu(source_in1.data(), source_in2.data(), source_cpu_results.data(), size);

    //Compute FPGA Results
    mmult_fpga(source_in1, source_in2, source_fpga_results, size);

    //Compare the results of FPGA to CPU 
    bool match = true;
    for (int i = 0 ; i < size * size; i++){
        if (source_fpga_results[i] != source_cpu_results[i]){
            std::cout << "Error: Result mismatch" << std::endl;
            std::cout << "i = " << i << " CPU result = " << source_cpu_results[i]
                << " FPGA result = " << source_fpga_results[i] << std::endl;
            match = false;
            break;
        }
    }

    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl; 

    return (match ? EXIT_SUCCESS :  EXIT_FAILURE);
}

