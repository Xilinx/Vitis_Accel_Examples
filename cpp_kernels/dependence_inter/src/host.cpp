/**
* Copyright (C) 2020 Xilinx, Inc
*
* Licensed under the Apache License, Version 2.0 (the "License"). You may
* not use this file except in compliance with the License. A copy of the
* License is located at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
* License for the specific language governing permissions and limitations
* under the License.
*/
#include "vconv.h"
#include "xcl2.hpp"
#include <algorithm>
#include <vector>

void vconv_sw(int* in, int* out, int height, int width) {
    int linebuf[K - 1][MAX_COLS] = {};
    int outIdx = 0;
    for (int col = 0; col < height; ++col) {
        for (int row = 0; row < width; ++row) {
            int in_val = in[col * width + row];
            int out_val = 0;
            for (int i = 0; i < K; i++) {
                int vwin_val = i < (K - 1) ? linebuf[i][row] : in_val;
                out_val += vwin_val * vcoeff[i];

                if (i > 0) linebuf[i - 1][row] = vwin_val;
            }
            out[outIdx++] = out_val;
        }
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];

    int testWidth = TEST_WIDTH;
    int testHeight = TEST_HEIGHT;
    int testSize = testHeight * testWidth;

    cl_int err;
    cl::Context context;
    cl::CommandQueue q;
    cl::Kernel krnl_vconv;
    // Allocate Memory in Host Memory
    size_t test_size_bytes = sizeof(int) * testSize;
    std::vector<int, aligned_allocator<int> > source_input(test_size_bytes);
    std::vector<int, aligned_allocator<int> > source_hw_results(test_size_bytes);
    std::vector<int, aligned_allocator<int> > source_sw_results(test_size_bytes);

    // Create the test data and Software Result
    std::generate(source_input.begin(), source_input.end(), std::rand);
    for (int i = 0; i < testSize; i++) {
        source_sw_results[i] = 0;
        source_hw_results[i] = 0;
    }

    // Running software vconv
    vconv_sw(source_input.data(), source_sw_results.data(), testHeight, testWidth);

    // OPENCL HOST CODE AREA START
    auto devices = xcl::get_xil_devices();
    // read_binary_file() is a utility API which will load the binaryFile
    // and will return the pointer to file buffer.
    auto fileBuf = xcl::read_binary_file(binaryFile);
    cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    bool valid_device = false;
    for (unsigned int i = 0; i < devices.size(); i++) {
        auto device = devices[i];
        // Creating Context and Command Queue for selected Device
        OCL_CHECK(err, context = cl::Context(device, nullptr, nullptr, nullptr, &err));
        OCL_CHECK(err, q = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err));

        std::cout << "Trying to program device[" << i << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
        cl::Program program(context, {device}, bins, nullptr, &err);
        if (err != CL_SUCCESS) {
            std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
        } else {
            std::cout << "Device[" << i << "]: program successful!\n";
            OCL_CHECK(err, krnl_vconv = cl::Kernel(program, "vconv", &err));
            valid_device = true;
            break; // we break because we found a valid device
        }
    }
    if (!valid_device) {
        std::cout << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }

    // Allocate Buffer in Global Memory
    OCL_CHECK(err, cl::Buffer buffer_input(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, test_size_bytes,
                                           source_input.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_output(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, test_size_bytes,
                                            source_hw_results.data(), &err));

    OCL_CHECK(err, err = krnl_vconv.setArg(0, buffer_input));
    OCL_CHECK(err, err = krnl_vconv.setArg(1, buffer_output));
    OCL_CHECK(err, err = krnl_vconv.setArg(2, testHeight));
    OCL_CHECK(err, err = krnl_vconv.setArg(3, testWidth));

    // Copy input data to device global memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_input}, 0 /* 0 means from host*/));

    // Launch the Kernel
    OCL_CHECK(err, err = q.enqueueTask(krnl_vconv));

    // Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_output}, CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();

    // OPENCL HOST CODE AREA END

    // Compare the results of the Device to the simulation
    bool match = true;

    std::cout << "Result = " << std::endl;
    for (int i = 0; i < testSize; i++) {
        if (source_hw_results[i] != source_sw_results[i]) {
            std::cout << "Error: Result mismatch" << std::endl;
            std::cout << "i = " << i << " CPU result = " << source_sw_results[i]
                      << " Device result = " << source_hw_results[i] << std::endl;
            match = false;
            break;
        }
    }

    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;
    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
