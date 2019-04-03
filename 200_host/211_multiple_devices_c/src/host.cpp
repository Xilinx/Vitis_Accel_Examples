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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <array>
#include <map>
#include <vector>

using std::array;
using std::map;
using std::vector;

cl::Program load_cl2_binary(cl::Program::Binaries, cl::Device device,
                          cl::Context context);
// This example demonstrates how to split work among multiple devices.
int main(int argc, char **argv) {

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];
    
    cl_int err = CL_SUCCESS;
    unsigned fileBufSize;

//OPENCL HOST CODE AREA START
    // get_xil_devices() is a utility API which will find the Xilinx
    // platforms and will return list of devices connected to Xilinx platform
    std::vector<cl::Device> devices = xcl::get_xil_devices();
    cl_uint device_count = devices.size();

    static const int elements_per_device = 1 << 10;
    static const int elements = elements_per_device * device_count;

    vector<int, aligned_allocator<int>> A(elements, 32);
    vector<int, aligned_allocator<int>> B(elements, 10);
    vector<int, aligned_allocator<int>> C(elements);

    // One element per device
    vector<cl::Context> contexts(device_count);
    vector<cl::Program> programs(device_count);
    vector<cl::Kernel> kernels(device_count);
    vector<cl::CommandQueue> queues(device_count);
    vector<std::string> device_name(device_count);

    vector<cl::Buffer> buffer_a(device_count);
    vector<cl::Buffer> buffer_b(device_count);
    vector<cl::Buffer> buffer_result(device_count);
    vector<cl::Program::Binaries> bins(device_count);
    vector<cl::Platform> platform;
    char* fileBuf[device_count];
    OCL_CHECK(err, err = cl::Platform::get(&platform));

    size_t size_per_device = elements_per_device * sizeof(int);
    cl_context_properties props[3] = {CL_CONTEXT_PLATFORM, (cl_context_properties)(platform[0])(), 0};
    std::cout <<"Initializing OpenCL objects" << std::endl;
    for (int d = 0; d < (int)device_count; d++) {
        // In this example. We will create a context for each of the devices
        std::cout << "Creating Context[" << d << "]..." << std::endl;
        OCL_CHECK(err, contexts[d] = cl::Context(devices[d], props, NULL, NULL, &err));
        OCL_CHECK(err, queues[d] = cl::CommandQueue(contexts[d], devices[d], CL_QUEUE_PROFILING_ENABLE, &err));
        OCL_CHECK(err, device_name[d] = devices[d].getInfo<CL_DEVICE_NAME>(&err));

        // read_binary_file() ia a utility API which will load the binaryFile
        // and will return pointer to file buffer.
        fileBuf[d] = xcl::read_binary_file(binaryFile, fileBufSize);
        bins[d].push_back({fileBuf[d], fileBufSize});
        programs[d] = load_cl2_binary(bins[d], devices[d], contexts[d]);
        OCL_CHECK(err, kernels[d] = cl::Kernel(programs[d], "vadd", &err));

        // Allocate Buffers in Global Memory
        // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and
        // Device-to-host communication
        size_t offset = d * elements_per_device;
        std::cout << "Creating Buffers[" << d << "]..." <<std::endl;
        OCL_CHECK(err, buffer_a[d] =
                cl::Buffer(contexts[d], CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, size_per_device, &A[offset], &err));
        OCL_CHECK(err, buffer_b[d] =
                cl::Buffer(contexts[d], CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, size_per_device, &B[offset], &err));
        OCL_CHECK(err, buffer_result[d] =
                cl::Buffer(contexts[d], CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, size_per_device, &C[offset], &err));
    }

    vector<cl::Event> events(device_count);
    for (int d = 0; d < (int)device_count; d++) {
        OCL_CHECK(err, err = kernels[d].setArg(0, buffer_result[d]));
        OCL_CHECK(err, err = kernels[d].setArg(1, buffer_a[d]));
        OCL_CHECK(err, err = kernels[d].setArg(2, buffer_b[d]));
        OCL_CHECK(err, err = kernels[d].setArg(3, elements_per_device));

        // Copy input data to device global memory
        std::cout << "Copying data..." << std::endl;
        OCL_CHECK(err, err = queues[d].enqueueMigrateMemObjects({buffer_a[d], buffer_b[d]}, 0/*0 means from host*/));

        // Launch the Kernel
        std::cout << "Launching Kernel..." << std::endl;
        OCL_CHECK(err, err = queues[d].enqueueTask(kernels[d], NULL, &events[d]));

        //Copy Result from Device Global Memory to Host Local Memory
        std::cout << "Getting Results..." << std::endl;
        OCL_CHECK(err, err = queues[d].enqueueMigrateMemObjects({buffer_result[d]}, CL_MIGRATE_MEM_OBJECT_HOST));
    }

    int dev = 0;
    for(auto queue : queues) {
      std::cout << "Waiting for work to finish on device " << dev++ << std::endl;
      OCL_CHECK(err, err = queue.flush());
      OCL_CHECK(err, err = queue.finish());
    }

    for (int d = 0; d < (int)device_count; d++) delete[] fileBuf[d];
//OPENCL HOST CODE AREA ENDS
    bool match = true;
     for (int i = 0; i < elements; i++) {
         int host_result = A[i] + B[i];
         if (C[i] != host_result){
             std::cout << "Error: Result mismatch" << std::endl;
             std::cout << "i = " << i << " CPU result = " << host_result << " Device result = " << C[i] << std::endl;
             match = false;
             break;
         }
     }
     std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;
     return (match ? EXIT_SUCCESS :  EXIT_FAILURE);
 }

cl::Program load_cl2_binary(cl::Program::Binaries bins, cl::Device device,
                          cl::Context context) {
    cl_int err;
    std::vector<cl::Device> devices(1, device);
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
    return program;
}
