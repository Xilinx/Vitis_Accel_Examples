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
    This is a simple to demonstrate Multi Process Support(MPS) using HLS kernels.

Procedure:
    To enable multi-process support, add the following entry to sdaccel.ini in 
    the same directory as all the executable(s):
    
    [Runtime]
    multiprocess=true

Limitation:
    Debug and Profile will not function correctly when multiprocess has been 
    enabled. 
    Emulation flow does not have support for multiprocess.

*******************************************************************************/

#include "xcl2.hpp"
#include <vector>
#include "multi_krnl.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


bool run_kernel(std::string& binaryFile, int krnl_id)
{
    cl_int err;
    unsigned fileBufSize;
    const char* krnl_names[]={"krnl_vadd", "krnl_vsub", "krnl_vmul"};
    
    int pid = getpid();
    printf("\n[PID: %d] Start Vector Operation (PARENT PPID: %d)\n", pid, getppid()); 
    
    size_t vector_size_bytes = sizeof(int) * LENGTH;
    std::vector<int,aligned_allocator<int>> source_a(LENGTH);
    std::vector<int,aligned_allocator<int>> source_b(LENGTH);
    std::vector<int,aligned_allocator<int>> result_sw(LENGTH);
    std::vector<int,aligned_allocator<int>> result_hw(LENGTH);

    /* Create the test data and run the vector addition locally */    
    for(int i=0; i < LENGTH; i++){
        source_a[i] = rand() % LENGTH;
        source_b[i] = rand() % LENGTH;
        result_hw[i] = 0;
        switch(krnl_id) {
            case 0: result_sw[i] = source_a[i] + source_b[i]; break;
            case 1: result_sw[i] = source_a[i] - source_b[i]; break;
            case 2: result_sw[i] = source_a[i] * source_b[i]; break;
            default: std::cout << "Kernel ID is unknown!!" << std::endl;
        }
    }

// OPENCL HOST CODE AREA START
    std::vector<cl::Device> devices = xcl::get_xil_devices();
    cl::Device device = devices[0];

    OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(err, cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
    std::string device_name = device.getInfo<CL_DEVICE_NAME>(); 

    printf("\n[PID: %d] Read XCLBIN file\n", pid); 

    char* fileBuf = xcl::read_binary_file(binaryFile, fileBufSize);
    cl::Program::Binaries bins{{fileBuf, fileBufSize}};
    
    devices.resize(1);
    
    printf("[PID: %d] Create a Program and a [ %s ] Kernel\n", pid, krnl_names[krnl_id]); 
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
    OCL_CHECK(err, cl::Kernel krnl(program, krnl_names[krnl_id], &err));

    OCL_CHECK(err, cl::Buffer buffer_a(context,CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, 
            vector_size_bytes, source_a.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_b(context,CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, 
            vector_size_bytes, source_b.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_c(context,CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, 
            vector_size_bytes, result_hw.data(), &err));
    
    /* Set the kernel arguments */
    int vector_length = LENGTH;
    
    OCL_CHECK(err, err = krnl.setArg(0, buffer_a));
    OCL_CHECK(err, err = krnl.setArg(1, buffer_b));
    OCL_CHECK(err, err = krnl.setArg(2, buffer_c));
    OCL_CHECK(err, err = krnl.setArg(3, vector_length));

    /* Copy input vectors to memory */
    printf("\n[PID: %d] Transfer the Input Data to Device\n", pid); 
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_a, buffer_b}, 0/* 0 means from host*/));

    /* Launch the kernel */
    printf("[PID: %d] Launch Kernel\n", pid); 
    OCL_CHECK(err, err = q.enqueueTask(krnl));
    
     /* Copy result to local buffer */
    printf("[PID: %d] Transfer the Output Data from Device\n", pid); 
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_c},CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();
// OPENCL HOST CODE AREA END

    /* Compare the results of the kernel to the simulation */
    bool krnl_match = true;
    
    printf("\n[PID: %d] Checking the Output Data with Golden Results...\n", pid); 
    
    for(int i = 0; i < LENGTH; i++) {
        if(result_sw[i] != result_hw[i]){
            printf("Error: i = %d CPU result = %d FPGA Result = %d\n", i, result_sw[i], result_hw[i]);
            krnl_match = false;
            break;
        }     
    }

    delete[] fileBuf;
    
    return krnl_match;
}

int main(int argc, char* argv[]) {

    int iter = 3;

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File> "<< std::endl;
        return EXIT_FAILURE;
    }
    
    std::string binaryFile = argv[1];
       
    //Setting XCL_MULTIPROCESS_MODE
    std::cout << "Set the env variable for Multi Process Support (MPS)" << std::endl;
    char mps_env[]="XCL_MULTIPROCESS_MODE=1";
    if(putenv( mps_env )!=0){
        std::cout << "putenv failed" << std::endl;
    }
    else 
        std::cout << "Env variable: XCL_MULTIPROCESS_MODE: " << getenv("XCL_MULTIPROCESS_MODE") << std::endl;


    bool result = true;
    
    std::cout << "Now create (" << iter << ") CHILD processes" << std::endl;
    for(int i=0; i<iter; i++) { 
        if(fork() == 0) {
            printf("[CHILD] PID %d from [PARENT] PPID %d\n",getpid(),getppid()); 
            result = run_kernel(binaryFile, i);
            exit(!(result));
        } 
    } 

    // Need to wait for all child process to complete
    for(int i=0; i<iter; i++) wait(NULL);
    std::cout << "\n[PID: " << getpid() << "] PARENT WAITS CHILD TO FINISH.\n\n" << std::endl;

    std::cout << "TEST " << ((result)? "PASSED" : "FAILED") << std::endl; 
    return ((result)? EXIT_SUCCESS : EXIT_FAILURE);
}
