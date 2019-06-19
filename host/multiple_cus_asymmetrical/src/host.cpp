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
In this example, we will demonstrate how each compute unit can be connected to different memory banks.
*******************************************************************/
#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

// This file is required for OpenCL C++ wrapper APIs
#include "xcl2.hpp"

auto constexpr c_test_size = 4 * 1024; //4KB data
auto constexpr NCU = 4;

////////////////////RESET FUNCTION//////////////////////////////////////////
int reset(int *a, int *b, int *sw_results, int *hw_results, unsigned int size) {
    //Fill the input vectors with data
    for (size_t i = 0; i < size; i++) {
        a[i] = rand() % size;
        b[i] = rand() % size;
        hw_results[i] = 0;
        sw_results[i] = a[i] + b[i];
    }
    return 0;
}
///////////////////VERIFY FUNCTION/////////////////////
bool verify(int *sw_results, int *hw_results, int size) {
    bool match = true;
    for (int i = 0; i < size; i++) {
        if (sw_results[i] != hw_results[i]) {
            match = false;
            break;
        }
    }
    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;
    return match;
}
////////MAIN FUNCTION//////////
int main(int argc, char **argv) {
    unsigned int size = c_test_size;

    // I/O Data Vectors
    std::vector<int, aligned_allocator<int>> source_in1(size);
    std::vector<int, aligned_allocator<int>> source_in2(size);
    std::vector<int, aligned_allocator<int>> source_hw_results(size);
    std::vector<int> sw_results(size);

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    // OpenCL Host Code Begins.
    cl_int err;

    // OpenCL objects
    cl::Device device;
    cl::Context context;
    cl::CommandQueue q;
    cl::Program program;

    auto binaryFile = argv[1];
    unsigned fileBufSize;

    // get_xil_devices() is a utility API which will find the xilinx
    // platforms and will return list of devices connected to Xilinx platform
    auto devices = xcl::get_xil_devices();

    // Selecting the first available Xilinx device
    device = devices[0];

    // Creating Context
    OCL_CHECK(err, context = cl::Context(device, NULL, NULL, NULL, &err));

    // Creating Command Queue
    OCL_CHECK(err,
              q = cl::CommandQueue(context,
                                   device,
                                   CL_QUEUE_PROFILING_ENABLE |
                                       CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE,
                                   &err));

    // read_binary_file() is a utility API which will load the binaryFile
    // and will return the pointer to file buffer.
    auto fileBuf = xcl::read_binary_file(binaryFile, fileBufSize);

    cl::Program::Binaries bins{{fileBuf, fileBufSize}};
    devices.resize(1);

    // Creating Program
    OCL_CHECK(err, program = cl::Program(context, devices, bins, NULL, &err));

    // Creating Kernel object using Compute unit names
    std::string cu_id;
    std::string krnl_name = "vadd";
    std::vector<cl::Kernel> krnls(NCU);
    for (int i = 0; i < NCU; i++) {
        cu_id = std::to_string(i + 1);
        std::string krnl_name_full = krnl_name + ":{" + "vadd_" + cu_id + "}";
        printf(
            "Creating a kernel [%s] for CU(%d)\n", krnl_name_full.c_str(), i);
        //Here Kernel object is created by specifying kernel name along with compute unit.
        //For such case, this kernel object can only access the specific Compute unit
        OCL_CHECK(err,
                  krnls[i] = cl::Kernel(program, krnl_name_full.c_str(), &err));
    }

    // Creating Buffers
    size_t vector_size_bytes = size * sizeof(int);
    std::vector<cl::Buffer> buffer_in1(NCU);
    std::vector<cl::Buffer> buffer_in2(NCU);
    std::vector<cl::Buffer> buffer_output(NCU);
    for (int i = 0; i < NCU; i++) {
        OCL_CHECK(err,
                  buffer_in1[i] =
                      cl::Buffer(context,
                                 CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                 vector_size_bytes,
                                 source_in1.data(),
                                 &err));
        OCL_CHECK(err,
                  buffer_in2[i] =
                      cl::Buffer(context,
                                 CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                 vector_size_bytes,
                                 source_in2.data(),
                                 &err));
        OCL_CHECK(err,
                  buffer_output[i] =
                      cl::Buffer(context,
                                 CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                                 vector_size_bytes,
                                 source_hw_results.data(),
                                 &err));
    }

    for (int i = 0; i < NCU; i++) {
        int narg = 0;

        //Setting Kernel arguments before migrating buffer object. This is must needed for asymetric compute
        // units case. Here Asymetric Compute unit means that each Compute unit is connected to different bank.
        OCL_CHECK(err, err = krnls[i].setArg(narg++, buffer_in1[i]));
        OCL_CHECK(err, err = krnls[i].setArg(narg++, buffer_in2[i]));
        OCL_CHECK(err, err = krnls[i].setArg(narg++, buffer_output[i]));
        OCL_CHECK(err, err = krnls[i].setArg(narg++, size));
        OCL_CHECK(err, err = krnls[i].setArg(narg++, i));
        OCL_CHECK(err, err = krnls[i].setArg(narg++, NCU));

        //Copy input data to device global memory
        OCL_CHECK(err,
                  err =
                      q.enqueueMigrateMemObjects({buffer_in1[i], buffer_in2[i]},
                                                 0 /* 0 means from host*/));

        //Launch the Kernel
        OCL_CHECK(err, err = q.enqueueTask(krnls[i]));
    }

    OCL_CHECK(err, err = q.finish());

    //Copy Result from Device Global Memory to Host Local Memory
    for (int i = 0; i < NCU; i++) {
        OCL_CHECK(err,
                  err = q.enqueueMigrateMemObjects({buffer_output[i]},
                                                   CL_MIGRATE_MEM_OBJECT_HOST));
    }
    OCL_CHECK(err, err = q.finish());

    bool match = verify(sw_results.data(), source_hw_results.data(), size);
    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
