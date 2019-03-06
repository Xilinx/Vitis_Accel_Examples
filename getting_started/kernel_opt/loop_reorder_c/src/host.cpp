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

/*******************************************************************************

Description: 

    SDx Vector Addition example which showcases the how reordering the loops
    helps achieve better pipeline initiation interval(II) and better performance.

*******************************************************************************/
//OpenCL utility layer include
#include "xcl2.hpp"
#include <vector>

//Array Size to access 
#define DATA_SIZE 64

//Maximum Array Size
#define MAX_SIZE 64

// Software implementation of Matrix Multiplication
// The inputs are of the size (DATA_SIZE x DATA_SIZE)
void m_softwareGold(
       std::vector<int,aligned_allocator<int>>& in1,   //Input Matrix 1
       std::vector<int,aligned_allocator<int>>& in2,   //Input Matrix 2
       std::vector<int,aligned_allocator<int>>& out    //Output Matrix
       )
{
    //Perform Matrix multiply Out = In1 x In2
    for(int i = 0; i < DATA_SIZE; i++) {
        for(int j = 0; j < DATA_SIZE; j++) {
            for(int k = 0; k < DATA_SIZE; k++) {
                out[i * DATA_SIZE + j] += in1[i * DATA_SIZE + k] * in2[k * DATA_SIZE + j];
            }
        }
    }
}  

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];

    //Allocate Memory in Host Memory
    if (DATA_SIZE > MAX_SIZE) {
        std::cout << "Size is bigger than internal buffer size,"
           << " please use a size smaller than " << MAX_SIZE << "!" << std::endl;
        return EXIT_FAILURE;
    }
    
    size_t matrix_size_bytes = sizeof(int) * DATA_SIZE * DATA_SIZE;
    cl_int err;
    unsigned fileBufSize;

    std::vector<int,aligned_allocator<int>> source_in1(matrix_size_bytes);
    std::vector<int,aligned_allocator<int>> source_in2(matrix_size_bytes);
    std::vector<int,aligned_allocator<int>> source_hw_results(matrix_size_bytes);
    std::vector<int,aligned_allocator<int>> source_sw_results(matrix_size_bytes);

    // Create the test data and Software Result 
    for(int i = 0 ; i < DATA_SIZE * DATA_SIZE ; i++){
        source_in1[i] = i;
        source_in2[i] = i * i;
        source_sw_results[i] = 0;
        source_hw_results[i] = 0;
    }

//OPENCL HOST CODE AREA START
    std::vector<cl::Device> devices = xcl::get_xil_devices();
    cl::Device device = devices[0];

    OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(err, cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
    OCL_CHECK(err, std::string device_name = device.getInfo<CL_DEVICE_NAME>(&err));

    char* fileBuf = xcl::read_binary_file(binaryFile, fileBufSize);
    cl::Program::Binaries bins{{fileBuf, fileBufSize}};
    devices.resize(1);
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
    OCL_CHECK(err, cl::Kernel krnl_loop_reorder(program,"mmult", &err));

    //Allocate Buffer in Global Memory
    OCL_CHECK(err, cl::Buffer buffer_in1(context,CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
            matrix_size_bytes,source_in1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_in2(context,CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
            matrix_size_bytes,source_in2.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_output(context,CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
            matrix_size_bytes,source_hw_results.data(), &err));

    int size = DATA_SIZE;

    OCL_CHECK(err, err = krnl_loop_reorder.setArg(0, buffer_in1));
    OCL_CHECK(err, err = krnl_loop_reorder.setArg(1, buffer_in2));
    OCL_CHECK(err, err = krnl_loop_reorder.setArg(2, buffer_output));
    OCL_CHECK(err, err = krnl_loop_reorder.setArg(3, size));

    //Copy input data to device global memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_in1, buffer_in2},0/* 0 means from host*/));

    //Launch the Kernel
    OCL_CHECK(err, err = q.enqueueTask(krnl_loop_reorder));
    q.finish();

    //Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_output},CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();

//OPENCL HOST CODE AREA END
 
    // Compute Software Results
    m_softwareGold(source_in1, source_in2, source_sw_results);

    // Compare the results of the Device to the simulation
    int match = 0;
    for (int i = 0 ; i < DATA_SIZE * DATA_SIZE ; i++){
        if (source_hw_results[i] != source_sw_results[i]){
            std::cout << "Error: Result mismatch" << std::endl;
            std::cout << "i = " << i << " CPU result = " << source_sw_results[i]
                << " Device result = " << source_hw_results[i] << std::endl;
            match = 1;
            break;
        }
    }

    delete[] fileBuf;

    std::cout << "TEST " << (match ? "FAILED" : "PASSED") << std::endl; 
    return (match ? EXIT_FAILURE :  EXIT_SUCCESS);
}
