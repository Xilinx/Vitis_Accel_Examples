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
* Changes from Lab 1:
    
Added local memory identifier in mmult.cpp for the local buffers and copied the data 
from/to DDR to/from local memory to reduce number of memory accesses.
*****/

//OpenCL utility layer include
#include "xcl2.hpp"
#include <stdlib.h>
#include <vector>

//Max Array Size
#define MAX_SIZE 64

//Array Size to access
#define DATA_SIZE 64

uint64_t get_duration_ns(const cl::Event &event) {
    cl_int err;
    uint64_t nstimestart, nstimeend;
    OCL_CHECK(err,
              err = event.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_START,
                                                     &nstimestart));
    OCL_CHECK(err,
              err = event.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_END,
                                                     &nstimeend));
    return (nstimeend - nstimestart);
}

//CPU implementation of Matrix Multiplication
//The inputs are of the size (DATA_SIZE x DATA_SIZE)
void mmult_cpu(int *in1, //Input Matrix 1
               int *in2, //Input Matrix 2
               int *out, //Output Matrix
               int dim   //One dimension of matrix
) {
    //Perform Matrix multiply Out = In1 x In2
    for (int i = 0; i < dim; i++) {
        for (int j = 0; j < dim; j++) {
            for (int k = 0; k < dim; k++) {
                out[i * dim + j] += in1[i * dim + k] * in2[k * dim + j];
            }
        }
    }
}

//Functionality to setup OpenCL context and trigger the Kernel
uint64_t mmult_fpga(
    std::vector<int, aligned_allocator<int>> &source_in1, //Input Matrix 1
    std::vector<int, aligned_allocator<int>> &source_in2, //Input Matrix 2
    std::vector<int, aligned_allocator<int>>
        &source_fpga_results, //Output Matrix
    int dim,                  //One dimension of matrix
    std::string &binaryFile   //Binary file string
) {
    cl_int err;
    unsigned fileBufSize;
    int size = dim;
    size_t matrix_size_bytes = sizeof(int) * size * size;

    //The get_xil_devices will return vector of Xilinx Devices
    auto devices = xcl::get_xil_devices();
    auto device = devices[0];

    //Creating Context and Command Queue for selected Device
    OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(
        err,
        cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
    auto device_name = device.getInfo<CL_DEVICE_NAME>();

    //read_binary() command will find the OpenCL binary file created using the
    //xocc compiler load into OpenCL Binary and return a pointer to file buffer
    //and it can contain many functions which can be executed on the
    //device.
    auto fileBuf = xcl::read_binary_file(binaryFile, fileBufSize);
    cl::Program::Binaries bins{{fileBuf, fileBufSize}};
    devices.resize(1);
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));

    //This call will extract a kernel out of the program we loaded in the
    //previous line. A kernel is an OpenCL function that is executed on the
    //FPGA. This function is defined in the src/mmult.cl file.
    OCL_CHECK(err, cl::Kernel kernel(program, "mmult", &err));

    //These commands will allocate memory on the FPGA. The cl::Buffer
    //objects can be used to reference the memory locations on the device.
    //The cl::Buffer object cannot be referenced directly and must be passed
    //to other OpenCL functions.
    OCL_CHECK(err,
              cl::Buffer buffer_in1(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    matrix_size_bytes,
                                    source_in1.data(),
                                    &err));
    OCL_CHECK(err,
              cl::Buffer buffer_in2(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    matrix_size_bytes,
                                    source_in2.data(),
                                    &err));
    OCL_CHECK(err,
              cl::Buffer buffer_output(context,
                                       CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                                       matrix_size_bytes,
                                       source_fpga_results.data(),
                                       &err));

    //Set the kernel arguments
    int narg = 0;
    OCL_CHECK(err, err = kernel.setArg(narg++, buffer_in1));
    OCL_CHECK(err, err = kernel.setArg(narg++, buffer_in2));
    OCL_CHECK(err, err = kernel.setArg(narg++, buffer_output));
    OCL_CHECK(err, err = kernel.setArg(narg++, size));

    //These commands will load the source_in1 and source_in2 vectors from the host
    //application into the buffer_in1 and buffer_in2 cl::Buffer objects. The data
    //will be be transferred from system memory over PCIe to the FPGA on-board
    //DDR memory.
    OCL_CHECK(err,
              err = q.enqueueMigrateMemObjects({buffer_in1, buffer_in2},
                                               0 /* 0 means from host*/));

    cl::Event event;
    uint64_t kernel_duration = 0;

    //Launch the kernel
    OCL_CHECK(err, err = q.enqueueTask(kernel, NULL, &event));

    //The result of the previous kernel execution will need to be retrieved in
    //order to view the results. This call will write the data from the
    //buffer_output cl_mem object to the source_fpga_results vector
    OCL_CHECK(err,
              err = q.enqueueMigrateMemObjects({buffer_output},
                                               CL_MIGRATE_MEM_OBJECT_HOST));
    OCL_CHECK(err, err = q.finish());
    delete[] fileBuf;

    kernel_duration = get_duration_ns(event);

    return kernel_duration;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];

    if (DATA_SIZE > MAX_SIZE) {
        std::cout << "Size is bigger than internal buffer size,"
                  << " please use a size smaller than " << MAX_SIZE << "!"
                  << std::endl;
        return EXIT_FAILURE;
    }

    //Allocate Memory in Host Memory
    int size = DATA_SIZE;
    size_t matrix_size_bytes = sizeof(int) * size * size;

    //When creating a buffer with user pointer, under the hood user ptr is
    //used if and only if it is properly aligned (page aligned). When not
    //aligned, runtime has no choice but to create its own host side buffer
    //that backs user ptr. This in turn implies that all operations that move
    //data to/from device incur an extra memcpy to move data to/from runtime's
    //own host buffer from/to user pointer. So it is recommended to use this
    //allocator if user wish to Create Buffer/Memory Object to align user buffer
    //to the page boundary. It will ensure that user buffer will be used when
    //user create Buffer/Mem Object.
    std::vector<int, aligned_allocator<int>> source_in1(matrix_size_bytes);
    std::vector<int, aligned_allocator<int>> source_in2(matrix_size_bytes);
    std::vector<int, aligned_allocator<int>> source_fpga_results(
        matrix_size_bytes);
    std::vector<int, aligned_allocator<int>> source_cpu_results(
        matrix_size_bytes);

    //Create the test data and Software Result
    for (int i = 0; i < DATA_SIZE * DATA_SIZE; i++) {
        source_in1[i] = rand() % size;
        source_in2[i] = rand() % size;
        source_cpu_results[i] = 0;
        source_fpga_results[i] = 0;
    }

    uint64_t kernel_duration = 0;

    //Compute CPU Results
    mmult_cpu(
        source_in1.data(), source_in2.data(), source_cpu_results.data(), size);

    //Compute FPGA Results
    kernel_duration = mmult_fpga(
        source_in1, source_in2, source_fpga_results, size, binaryFile);

    //Compare the results of the FPGA to CPU
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

    std::cout << "Wall Clock Time (Kernel execution): " << kernel_duration
              << std::endl;
    std::cout << "Note: Wall Clock Time is meaningful for real hardware "
                 "execution only,"
              << "not for emulation." << std::endl;

    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
