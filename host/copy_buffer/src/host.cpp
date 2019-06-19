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

using std::vector;

static const int DATA_SIZE = 1024;
static const std::string error_message =
    "Error: Result mismatch:\n"
    "i = %d CPU result = %d Device result = %d\n";

// This example illustrates the simple OpenCL example that performs
// buffer copy from one buffer to another
int main(int argc, char **argv) {

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];
    cl_int err;
    unsigned fileBufSize;
    size_t size_in_bytes = DATA_SIZE * sizeof(int);

    vector<int, aligned_allocator<int>> source_a(DATA_SIZE, 13);
    vector<int, aligned_allocator<int>> source_results(DATA_SIZE);

    auto devices = xcl::get_xil_devices();
    auto device = devices[0];

    OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(
        err,
        cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
    auto device_name = device.getInfo<CL_DEVICE_NAME>();
    std::cout << "Found Device=" << device_name.c_str() << std::endl;

    auto fileBuf = xcl::read_binary_file(binaryFile, fileBufSize);
    cl::Program::Binaries bins{{fileBuf, fileBufSize}};
    devices.resize(1);
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));

    OCL_CHECK(err,
              cl::Buffer buffer_a(context,
                                  CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                  size_in_bytes,
                                  source_a.data(),
                                  &err));
    OCL_CHECK(err,
              cl::Buffer buffer_result(context,
                                       CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                                       size_in_bytes,
                                       source_results.data(),
                                       &err));
    OCL_CHECK(err,
              cl::Buffer buffer_b(
                  context, CL_MEM_READ_ONLY, size_in_bytes, NULL, &err));

    OCL_CHECK(err, cl::Kernel kernel(program, "vector_add", &err));

    OCL_CHECK(err, err = kernel.setArg(0, buffer_result));
    OCL_CHECK(err, err = kernel.setArg(1, buffer_a));
    OCL_CHECK(err, err = kernel.setArg(2, buffer_b));
    OCL_CHECK(err, err = kernel.setArg(3, DATA_SIZE));

    //copy buffer a to device.
    OCL_CHECK(
        err,
        err = q.enqueueMigrateMemObjects({buffer_a}, 0 /* 0 means from host*/));

    // This enqueueCopyBuffer() command will copy buffer from buffer_a to buffer_b
    OCL_CHECK(err,
              err =
                  q.enqueueCopyBuffer(buffer_a, buffer_b, 0, 0, size_in_bytes));

    // This function will execute the kernel on the FPGA
    OCL_CHECK(err, err = q.enqueueNDRangeKernel(kernel, 0, 1, 1, NULL, NULL));

    OCL_CHECK(err,
              err = q.enqueueMigrateMemObjects({buffer_result},
                                               CL_MIGRATE_MEM_OBJECT_HOST));
    OCL_CHECK(err, err = q.finish());

    int match = 0;
    printf("Result = \n");
    for (int i = 0; i < DATA_SIZE; i++) {
        int host_result = source_a[i] + source_a[i];
        if (source_results[i] != host_result) {
            printf(error_message.c_str(), i, host_result, source_results[i]);
            match = 1;
            break;
        } else {
            printf("%d ", source_results[i]);
            if (((i + 1) % 16) == 0)
                printf("\n");
        }
    }

    delete[] fileBuf;

    std::cout << "TEST " << (match ? "FAILED" : "PASSED") << std::endl;
    return (match ? EXIT_FAILURE : EXIT_SUCCESS);
}
