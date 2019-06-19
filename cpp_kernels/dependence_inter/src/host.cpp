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
#include "vconv.h"
#include "xcl2.hpp"
#include <vector>

void vconv_sw(int *in, int *out, int height, int width) {
    int linebuf[K - 1][MAX_COLS] = {};
    int outIdx = 0;
    for (int col = 0; col < height; ++col) {
        for (int row = 0; row < width; ++row) {
            int in_val = in[col * width + row];
            int out_val = 0;
            for (int i = 0; i < K; i++) {
                int vwin_val = i < (K - 1) ? linebuf[i][row] : in_val;
                out_val += vwin_val * vcoeff[i];

                if (i > 0)
                    linebuf[i - 1][row] = vwin_val;
            }
            out[outIdx++] = out_val;
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];

    int testWidth = TEST_WIDTH;
    int testHeight = TEST_HEIGHT;
    int testSize = testHeight * testWidth;

    cl_int err;
    unsigned fileBufSize;
    //Allocate Memory in Host Memory
    size_t test_size_bytes = sizeof(int) * testSize;
    std::vector<int, aligned_allocator<int>> source_input(test_size_bytes);
    std::vector<int, aligned_allocator<int>> source_hw_results(test_size_bytes);
    std::vector<int, aligned_allocator<int>> source_sw_results(test_size_bytes);

    // Create the test data and Software Result
    for (int i = 0; i < testSize; i++) {
        source_input[i] = rand() % testSize;
        source_sw_results[i] = 0;
        source_hw_results[i] = 0;
    }

    //Running software vconv
    vconv_sw(
        source_input.data(), source_sw_results.data(), testHeight, testWidth);

    //OPENCL HOST CODE AREA START
    auto devices = xcl::get_xil_devices();
    auto device = devices[0];

    OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(
        err,
        cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
    OCL_CHECK(err,
              std::string device_name = device.getInfo<CL_DEVICE_NAME>(&err));
    std::cout << "Found Device=" << device_name.c_str() << std::endl;

    auto fileBuf = xcl::read_binary_file(binaryFile, fileBufSize);
    cl::Program::Binaries bins{{fileBuf, fileBufSize}};
    devices.resize(1);
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
    OCL_CHECK(err, cl::Kernel krnl_vconv(program, "vconv", &err));

    //Allocate Buffer in Global Memory
    OCL_CHECK(err,
              cl::Buffer buffer_input(context,
                                      CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                      test_size_bytes,
                                      source_input.data(),
                                      &err));
    OCL_CHECK(err,
              cl::Buffer buffer_output(context,
                                       CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                                       test_size_bytes,
                                       source_hw_results.data(),
                                       &err));

    OCL_CHECK(err, err = krnl_vconv.setArg(0, buffer_input));
    OCL_CHECK(err, err = krnl_vconv.setArg(1, buffer_output));
    OCL_CHECK(err, err = krnl_vconv.setArg(2, testHeight));
    OCL_CHECK(err, err = krnl_vconv.setArg(3, testWidth));

    //Copy input data to device global memory
    OCL_CHECK(err,
              err = q.enqueueMigrateMemObjects({buffer_input},
                                               0 /* 0 means from host*/));

    //Launch the Kernel
    OCL_CHECK(err, err = q.enqueueTask(krnl_vconv));

    //Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err,
              err = q.enqueueMigrateMemObjects({buffer_output},
                                               CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();

    //OPENCL HOST CODE AREA END

    // Compare the results of the Device to the simulation
    bool match = true;

    std::cout << "Result = " << std::endl;
    for (int i = 0; i < testSize; i++) {
        if (source_hw_results[i] != source_sw_results[i]) {
            std::cout << "Error: Result mismatch" << std::endl;
            std::cout << "i = " << i << " CPU result = " << source_sw_results[i]
                      << " Device result = " << source_hw_results[i]
                      << std::endl;
            match = false;
            break;
        }
    }

    delete[] fileBuf;

    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;
    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
