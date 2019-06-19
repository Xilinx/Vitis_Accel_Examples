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
//OpenCL utility layer include
#include "host.h"
#include "xcl2.hpp"
#include <vector>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];

    //Allocate Memory in Host Memory
    size_t vector_size_bytes = sizeof(DTYPE) * BLOCK_SIZE;
    std::vector<DTYPE, aligned_allocator<DTYPE>> a(
        BLOCK_SIZE); // original data set given to device
    std::vector<DTYPE, aligned_allocator<DTYPE>> c(
        BLOCK_SIZE); // results returned from device
    std::vector<DTYPE, aligned_allocator<DTYPE>> sw_c(
        BLOCK_SIZE); // results returned from software

    // Create the test data and Software Result
    int alpha = 3;
    for (int i = 0; i < BLOCK_SIZE; i++) {
        a[i] = i;
        c[i] = 0;
        sw_c[i] = alpha * a[i];
    }

    //OPENCL HOST CODE AREA START
    cl_int err;
    unsigned fileBufSize;
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
    OCL_CHECK(
        err, cl::Kernel krnl_window_array_2d(program, "window_array_2d", &err));

    //Allocate Buffer in Global Memory
    OCL_CHECK(err,
              cl::Buffer buffer_a(context,
                                  CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                  vector_size_bytes,
                                  a.data(),
                                  &err));
    OCL_CHECK(err,
              cl::Buffer buffer_c(context,
                                  CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                                  vector_size_bytes,
                                  c.data(),
                                  &err));

    //Set the Kernel Arguments
    int nargs = 0;
    OCL_CHECK(err, err = krnl_window_array_2d.setArg(nargs++, buffer_a));
    OCL_CHECK(err, err = krnl_window_array_2d.setArg(nargs++, buffer_c));
    OCL_CHECK(err, err = krnl_window_array_2d.setArg(nargs++, alpha));

    //Copy input data to device global memory
    OCL_CHECK(
        err,
        err = q.enqueueMigrateMemObjects({buffer_a}, 0 /* 0 means from host*/));

    //Launch the Kernel
    OCL_CHECK(err, err = q.enqueueTask(krnl_window_array_2d));

    //Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err,
              err = q.enqueueMigrateMemObjects({buffer_c},
                                               CL_MIGRATE_MEM_OBJECT_HOST));
    OCL_CHECK(err, err = q.finish());

    //OPENCL HOST CODE AREA END

    // Validate
    unsigned int correct = 0; // number of correct results returned
    for (int i = 0; i < BLOCK_SIZE; i++) {
        if (c[i] == sw_c[i]) {
            correct++;
        } else {
            std::cout << std::endl
                      << " wrong sw " << sw_c[i] << " hw " << c[i] << ": index "
                      << std::endl;
        }
    }

    delete[] fileBuf;

    // Print a brief summary detailing the results
    std::cout << "Computed '" << correct << "/" << BLOCK_SIZE
              << "' correct values!" << std::endl;

    std::cout << "TEST " << (correct != BLOCK_SIZE ? "FAILED" : "PASSED")
              << std::endl;
    return (correct != BLOCK_SIZE ? EXIT_FAILURE : EXIT_SUCCESS);
}
