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
#include <algorithm>
#include <vector>
#define DATA_SIZE 4096

/*******************************************************
 * For most of the cases implementation tools will place a kernel in the same super logic region (SLR)
 * as the memory bank that it access.
 * If competition for logic resources leads to situations where a kernel is not automatically
 * placed in the correct SLR or one SLR is overbooked, user needs to move few kernels to different SLR.
 * For such cases, user has to provide SLR assignment specific XOCC option to move kernel to respective
 * SLR region 
 *
 * In this example we have two kernels (vmult, vadd), in which first(vmult) is assigned to SLR0 and
 * second(vadd) is assigned to SLR1 as below command of xocc link command:
 * --slr vmult_1:SLR0 --slr vadd_1:SLR1
 *******************************************************/

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];

    size_t vector_size_bytes = sizeof(int) * DATA_SIZE;
    cl_int err;
    std::vector<int, aligned_allocator<int>> A(DATA_SIZE);
    std::vector<int, aligned_allocator<int>> B(DATA_SIZE);
    std::vector<int, aligned_allocator<int>> temp(DATA_SIZE);
    std::vector<int, aligned_allocator<int>> C(DATA_SIZE);

    // Create the test data
    std::generate(A.begin(), A.end(), std::rand);
    std::generate(B.begin(), B.end(), std::rand);
    std::generate(temp.begin(), temp.end(), std::rand);
    std::generate(C.begin(), C.end(), std::rand);

    auto devices = xcl::get_xil_devices();
    auto device = devices[0];

    int size = DATA_SIZE;
    OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(
        err,
        cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
    OCL_CHECK(err,
              std::string device_name = device.getInfo<CL_DEVICE_NAME>(&err));

    // The temporary pointer(temp) is created mainly for the dynamic platforms,
    // since in the dynamic platforms we have to release all cl buffers before
    // calling cl::Program a second time in the same process. The code block below
    // is in the braces because the cl objects are automatically released once
    // the block ends.
    {
        printf("INFO: loading vmul kernel\n");

       auto fileBuf = xcl::read_binary_file(binaryFile);
       cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
        devices.resize(1);
        OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
        OCL_CHECK(err, cl::Kernel vector_mult(program, "vmult", &err));

        OCL_CHECK(err,
                  cl::Buffer buffer_in1(context,
                                        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                        vector_size_bytes,
                                        A.data(),
                                        &err));
        OCL_CHECK(err,
                  cl::Buffer buffer_in2(context,
                                        CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                        vector_size_bytes,
                                        B.data(),
                                        &err));
        OCL_CHECK(
            err,
            cl::Buffer buffer_mul_out(context,
                                      CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                      vector_size_bytes,
                                      temp.data(),
                                      &err));

        OCL_CHECK(err, err = vector_mult.setArg(0, buffer_in1));
        OCL_CHECK(err, err = vector_mult.setArg(1, buffer_in2));
        OCL_CHECK(err, err = vector_mult.setArg(2, buffer_mul_out));
        OCL_CHECK(err, err = vector_mult.setArg(3, size));

        // Copy input data to device global memory
        OCL_CHECK(err,
                  err = q.enqueueMigrateMemObjects({buffer_in1, buffer_in2},
                                                   0 /* 0 means from host*/));

        // Launch the Kernel
        OCL_CHECK(err, err = q.enqueueTask(vector_mult));

        OCL_CHECK(err,
                  err = q.enqueueMigrateMemObjects({buffer_mul_out},
                                                   CL_MIGRATE_MEM_OBJECT_HOST));
        q.finish();
    }
    {
        printf("loading vadd kernel\n");
       auto fileBuf = xcl::read_binary_file(binaryFile);
       cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
        devices.resize(1);
        OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
        OCL_CHECK(err, cl::Kernel vector_add(program, "vadd", &err));

        OCL_CHECK(err,
                  cl::Buffer d_temp(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    vector_size_bytes,
                                    temp.data(),
                                    &err));
        OCL_CHECK(
            err,
            cl::Buffer buffer_vadd_out(context,
                                       CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                                       vector_size_bytes,
                                       C.data(),
                                       &err));

        OCL_CHECK(err, err = vector_add.setArg(0, d_temp));
        OCL_CHECK(err, err = vector_add.setArg(1, d_temp));
        OCL_CHECK(err, err = vector_add.setArg(2, buffer_vadd_out));
        OCL_CHECK(err, err = vector_add.setArg(3, size));

        OCL_CHECK(err,
                  err = q.enqueueMigrateMemObjects({d_temp},
                                                   0 /* 0 means from host*/));

        OCL_CHECK(err, err = q.enqueueTask(vector_add));
        OCL_CHECK(err,
                  err = q.enqueueMigrateMemObjects({buffer_vadd_out},
                                                   CL_MIGRATE_MEM_OBJECT_HOST));
        q.finish();
    }
    // OPENCL HOST CODE AREA END

    // Compare the results of the Device to the simulation
    bool match = true;
    for (int i = 0; i < DATA_SIZE; i++) {
        if ((A[i] * B[i] + temp[i]) != C[i]) {
            std::cout << "Error: Result mismatch" << std::endl;
            std::cout << "i = " << i
                      << " CPU result = " << (A[i] * B[i] + temp[i])
                      << " Device result = " << C[i] << std::endl;
            match = false;
            break;
        }
    }

    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;
    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
