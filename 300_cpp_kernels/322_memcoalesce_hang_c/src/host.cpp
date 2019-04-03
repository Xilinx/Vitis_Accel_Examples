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

#define LENGTH 8
#define ITERATION 64
void dummy_op(  std::vector<int,aligned_allocator<int>> & din1,
                std::vector<int,aligned_allocator<int>> &din2, 
                std::vector<int,aligned_allocator<int>> &din3,
                std::vector<int,aligned_allocator<int>> &dout)
{
    for (int i = 0; i < ITERATION; i++) {
        int sum = 0;
        for (int j = 0; j < LENGTH; j++){
            sum +=  din1[i*LENGTH+ j] + din2[i*LENGTH+ j] + din3[j];
        }
        dout[i] = sum;
    }
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];

    size_t total_size = (LENGTH) * (ITERATION);
    //Allocate Memory in Host Memory
    std::vector<int,aligned_allocator<int>> din1(total_size);
    std::vector<int,aligned_allocator<int>> din2(total_size);
    std::vector<int,aligned_allocator<int>> din3(ITERATION);
    std::vector<int,aligned_allocator<int>> dout(ITERATION);
    std::vector<int,aligned_allocator<int>> result_krnl(ITERATION);

    for (size_t i = 0 ; i < total_size; i++ ){
        din1[i] = i; 
        din2[i] = i + 2;
        if (i < ITERATION){
            din3[i]  = i + 3;
            dout[i] = 0;
        }
    }
    dummy_op(din1,din2,din3,dout);

    cl_int err;
    unsigned fileBufSize;
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
    OCL_CHECK(err, cl::Kernel krnl(program,"dummy_op", &err));

    //Allocate Buffer in Global Memory
    OCL_CHECK(err, cl::Buffer buffer_din1  (context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
            sizeof(int) * total_size, din1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_din2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
            sizeof(int) * total_size, din2.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_din3  (context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
            sizeof(int) * (ITERATION), din3.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_dout  (context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
            sizeof(int) * (ITERATION), result_krnl.data(), &err));

    /* Set the kernel arguments */
    int nargs=0;
    OCL_CHECK(err, err = krnl.setArg(nargs++,buffer_din1));
    OCL_CHECK(err, err = krnl.setArg(nargs++,buffer_din2));
    OCL_CHECK(err, err = krnl.setArg(nargs++,buffer_din3));
    OCL_CHECK(err, err = krnl.setArg(nargs++,buffer_dout));

    //Copy input RGB Image to device global memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_din1, buffer_din2, buffer_din3},0/* 0 means from host*/));

    /* Launch the kernel */
    OCL_CHECK(err, err = q.enqueueTask(krnl));

     /* Copy result to local buffer */
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_dout},CL_MIGRATE_MEM_OBJECT_HOST));
    OCL_CHECK(err, err = q.finish());

    /* Compare the results of the kernel to the simulation */
    int krnl_match = 0;
    for(int i = 0; i < ITERATION; i++){
        if(dout[i] != result_krnl[i]){
            std::cout << "Error: Result mismatch" << std::endl;
            std::cout << "i = " << i << " CPU result = " << dout[i]
                << " Krnl Result = " << result_krnl[i] << std::endl;
            krnl_match = 1;
            break;
        }
    }

    delete[] fileBuf;

    std::cout << "TEST " << (krnl_match ? "FAILED" : "PASSED") << std::endl; 
    return (krnl_match ? EXIT_FAILURE :  EXIT_SUCCESS);
}
