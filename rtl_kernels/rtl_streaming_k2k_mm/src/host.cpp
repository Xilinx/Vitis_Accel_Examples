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
#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

// This extension file is required for stream APIs
#include "CL/cl_ext_xilinx.h"
// This file is required for OpenCL C++ wrapper APIs
#include "xcl2.hpp"

auto constexpr c_test_size = 1 * 1024 * 1024; // 1 MB data

////////MAIN FUNCTION//////////
int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }
    int size = c_test_size;

    if (xcl::is_hw_emulation()) {
        size = 4096; // 4KB for HW emulation
    } else if (xcl::is_emulation()) {
        size = 2 * 1024 * 1024; // 2MB for sw emulation
    }

    // I/O Data Vectors
    std::vector<int, aligned_allocator<int> > a(size);
    std::vector<int, aligned_allocator<int> > hw_results(size);
    std::vector<int> sw_results(size);

    std::generate(a.begin(), a.end(), std::rand);

    for (int i = 0; i < size; i++) {
        sw_results[i] = a[i] + 2;
    }

    // OpenCL Host Code Begins.
    cl_int err;

    // OpenCL objects
    cl::Device device;
    cl::Context context;
    cl::CommandQueue q;
    cl::Program program;
    cl::Kernel krnl_myadder1;
    cl::Kernel krnl_myadder2;
    cl::Kernel krnl_mm2s, krnl_s2mm;

    auto binaryFile = argv[1];

    // get_xil_devices() is a utility API which will find the xilinx
    // platforms and will return list of devices connected to Xilinx platform
    auto devices = xcl::get_xil_devices();

    // read_binary_file() is a utility API which will load the binaryFile
    // and will return the pointer to file buffer.
    auto fileBuf = xcl::read_binary_file(binaryFile);
    cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    bool valid_device = false;
    for (unsigned int i = 0; i < devices.size(); i++) {
        device = devices[i];
        // Creating Context and Command Queue for selected Device
        OCL_CHECK(err, context = cl::Context(device, nullptr, nullptr, nullptr, &err));
        OCL_CHECK(err, q = cl::CommandQueue(context, device,
                                            CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &err));
        std::cout << "Trying to program device[" << i << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
        cl::Program program(context, {device}, bins, nullptr, &err);
        if (err != CL_SUCCESS) {
            std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
        } else {
            std::cout << "Device[" << i << "]: program successful!\n";
            // Creating Kernel
            OCL_CHECK(err, krnl_myadder1 = cl::Kernel(program, "myadder1:{myadder1_1}", &err));
            OCL_CHECK(err, krnl_myadder2 = cl::Kernel(program, "myadder2:{myadder2_1}", &err));
            OCL_CHECK(err, krnl_s2mm = cl::Kernel(program, "krnl_s2mm", &err));
            OCL_CHECK(err, krnl_mm2s = cl::Kernel(program, "krnl_mm2s", &err));
            valid_device = true;
            break; // we break because we found a valid device
        }
    }
    if (!valid_device) {
        std::cout << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }

    std::cout << "Vector Increment of elements 0x" << std::hex << size << " by 2" << std::endl;

    // Running the kernel
    unsigned int vector_size_bytes = size * sizeof(int);

    // Allocate Buffer in Global Memory
    // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and
    // Device-to-host communication
    OCL_CHECK(err, cl::Buffer buffer_input(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, vector_size_bytes, a.data(),
                                           &err));
    OCL_CHECK(err, cl::Buffer buffer_output(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, vector_size_bytes,
                                            hw_results.data(), &err));

    // Setting Kernel Arguments
    OCL_CHECK(err, err = krnl_mm2s.setArg(0, buffer_input));
    OCL_CHECK(err, err = krnl_mm2s.setArg(2, size));
    OCL_CHECK(err, err = krnl_s2mm.setArg(0, buffer_output));
    OCL_CHECK(err, err = krnl_s2mm.setArg(2, size));

    // Copy input data to device global memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_input}, 0 /* 0 means from host*/));
    q.finish();

    // Launch the Kernel
    OCL_CHECK(err, err = q.enqueueTask(krnl_mm2s));
    OCL_CHECK(err, err = q.enqueueTask(krnl_myadder1));
    OCL_CHECK(err, err = q.enqueueTask(krnl_myadder2));
    OCL_CHECK(err, err = q.enqueueTask(krnl_s2mm));

    q.finish(); // Waiting for kernels to finish execution

    // Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_output}, CL_MIGRATE_MEM_OBJECT_HOST));

    // OpenCL Host Code Ends
    q.finish();

    // Compare the device results with software results
    bool match = std::equal(sw_results.begin(), sw_results.end(), hw_results.begin());
    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;

    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
