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
#include "host.h"
#include "xcl2.hpp"
#include <vector>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];

    // Allocate result buffer on host memory
    size_t vector_size_bytes = sizeof(int) * LENGTH;
    std::vector<int, aligned_allocator<int>> source_a(4 * LENGTH);
    std::vector<int, aligned_allocator<int>> result_sim(LENGTH);
    std::vector<int, aligned_allocator<int>> result_krnl(LENGTH);

    // Create the test data and run the vector addition locally
    for (int i = 0; i < LENGTH; i++) {
        source_a[4 * i] = i;
        source_a[4 * i + 1] = 2 * i;
        source_a[4 * i + 2] = 3 * i;
        source_a[4 * i + 3] = 4 * i;
        result_sim[i] = source_a[4 * i] + source_a[4 * i + 1] +
                        source_a[4 * i + 2] + source_a[4 * i + 3];
    }

    // OPENCL HOST CODE AREA START
    cl_int err;
    auto devices = xcl::get_xil_devices();
    auto device = devices[0];

    OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(
        err,
        cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
    std::string device_name = device.getInfo<CL_DEVICE_NAME>();

   auto fileBuf = xcl::read_binary_file(binaryFile);
   cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    devices.resize(1);
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
    OCL_CHECK(err, cl::Kernel krnl(program, "krnl_vadd", &err));

    OCL_CHECK(err,
              cl::Buffer buffer_a(context,
                                  CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                  4 * vector_size_bytes,
                                  source_a.data(),
                                  &err));
    OCL_CHECK(err,
              cl::Buffer buffer_e(context,
                                  CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                                  vector_size_bytes,
                                  result_krnl.data(),
                                  &err));

    // Set the kernel arguments
    int vector_length = LENGTH;

    OCL_CHECK(err, err = krnl.setArg(0, buffer_a));
    OCL_CHECK(err, err = krnl.setArg(1, buffer_e));
    OCL_CHECK(err, err = krnl.setArg(2, vector_length));

    // Copy input vectors to memory
    OCL_CHECK(
        err,
        err = q.enqueueMigrateMemObjects({buffer_a}, 0 /* 0 means from host*/));

    // Launch the kernel and get profile data (stop-start)
    uint64_t nstimestart, nstimeend;
    cl::Event event;
    OCL_CHECK(err, err = q.enqueueTask(krnl, NULL, &event));
    OCL_CHECK(err, err = q.finish());
    OCL_CHECK(err,
              err = event.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_START,
                                                     &nstimestart));
    OCL_CHECK(err,
              err = event.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_END,
                                                     &nstimeend));
    auto duration_nanosec = nstimeend - nstimestart;
    std::cout << " **** Duration returned by profile API is "
              << (duration_nanosec * (1.0e-6)) << " ms **** " << std::endl;

    // Copy result to local buffer
    OCL_CHECK(err,
              err = q.enqueueMigrateMemObjects({buffer_e},
                                               CL_MIGRATE_MEM_OBJECT_HOST));
    OCL_CHECK(err, err = q.finish());

    // OPENCL HOST CODE AREA END

    // Compare the results of the kernel to the simulation
    int krnl_match = 0;
    for (int i = 0; i < LENGTH; i++) {
        if (result_sim[i] != result_krnl[i]) {
            std::cout << "Error: Result mismatch" << std::endl;
            std::cout << "i = " << i << " CPU result = " << result_sim[i]
                      << " Krnl Result = " << result_krnl[i] << std::endl;
            krnl_match = 1;
            break;
        } else {
            ; // nothing...Or you could print additional debug message with result matched
        }
    }


    std::cout << "TEST " << (krnl_match ? "FAILED" : "PASSED") << std::endl;
    return (krnl_match ? EXIT_FAILURE : EXIT_SUCCESS);
}
