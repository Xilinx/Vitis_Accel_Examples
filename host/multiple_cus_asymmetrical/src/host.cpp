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
// In this example, we will demonstrate how each compute unit can be connected
// to different memory banks.

#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
// This file is required for OpenCL C++ wrapper APIs
#include "xcl2.hpp"

auto constexpr c_test_size = 4 * 4096; // 16KB data
auto constexpr NCU = 4;

////////////////////RESET FUNCTION//////////////////////////////////////////
int reset(int* a, int* b, int* sw_results, int* hw_results, unsigned int size) {
    // Fill the input vectors with data
    for (size_t i = 0; i < size; i++) {
        a[i] = rand() % size;
        b[i] = rand() % size;
        hw_results[i] = 0;
        sw_results[i] = a[i] + b[i];
    }
    return 0;
}
///////////////////VERIFY FUNCTION/////////////////////
bool verify(int* sw_results, int* hw_results, int size) {
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
int main(int argc, char** argv) {
    unsigned int size = c_test_size;
    unsigned int chunk_size = size / NCU;

    // I/O Data Vectors
    std::vector<int, aligned_allocator<int> > source_in1(size);
    std::vector<int, aligned_allocator<int> > source_in2(size);
    std::vector<int, aligned_allocator<int> > source_hw_results(size);
    std::vector<int> sw_results(size);

    reset(source_in1.data(), source_in2.data(), sw_results.data(), source_hw_results.data(), size);

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
    std::string cu_id;
    std::string krnl_name = "vadd";
    std::vector<cl::Kernel> krnls(NCU);

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
        auto device = devices[i];
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
            // Creating Kernel object using Compute unit names
            for (int i = 0; i < NCU; i++) {
                cu_id = std::to_string(i + 1);
                std::string krnl_name_full = krnl_name + ":{" + "vadd_" + cu_id + "}";
                printf("Creating a kernel [%s] for CU(%d)\n", krnl_name_full.c_str(), i);
                // Here Kernel object is created by specifying kernel name along with
                // compute unit.
                // For such case, this kernel object can only access the specific
                // Compute unit
                OCL_CHECK(err, krnls[i] = cl::Kernel(program, krnl_name_full.c_str(), &err));
            }
            valid_device = true;
            break; // we break because we found a valid device
        }
    }
    if (!valid_device) {
        std::cout << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }

    // Creating Buffers
    size_t vector_size_bytes = chunk_size * sizeof(int);
    std::vector<cl::Buffer> buffer_in1(NCU);
    std::vector<cl::Buffer> buffer_in2(NCU);
    std::vector<cl::Buffer> buffer_output(NCU);
    for (int i = 0; i < NCU; i++) {
        OCL_CHECK(err, buffer_in1[i] = cl::Buffer(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, vector_size_bytes,
                                                  source_in1.data() + (i * chunk_size), &err));
        OCL_CHECK(err, buffer_in2[i] = cl::Buffer(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, vector_size_bytes,
                                                  source_in2.data() + (i * chunk_size), &err));
        OCL_CHECK(err,
                  buffer_output[i] = cl::Buffer(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, vector_size_bytes,
                                                source_hw_results.data() + (i * chunk_size), &err));
    }

    for (int i = 0; i < NCU; i++) {
        int narg = 0;

        // Setting Kernel arguments before migrating buffer object. This is must
        // needed for asymetric compute
        // units case. Here Asymetric Compute unit means that each Compute unit is
        // connected to different bank.
        OCL_CHECK(err, err = krnls[i].setArg(narg++, buffer_in1[i]));
        OCL_CHECK(err, err = krnls[i].setArg(narg++, buffer_in2[i]));
        OCL_CHECK(err, err = krnls[i].setArg(narg++, buffer_output[i]));
        OCL_CHECK(err, err = krnls[i].setArg(narg++, chunk_size));

        // Copy input data to device global memory
        OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_in1[i], buffer_in2[i]}, 0 /* 0 means from host*/));
    }
    OCL_CHECK(err, err = q.finish());

    // Launch the Kernel
    for (int i = 0; i < NCU; i++) {
        OCL_CHECK(err, err = q.enqueueTask(krnls[i]));
    }
    OCL_CHECK(err, err = q.finish());

    // Copy Result from Device Global Memory to Host Local Memory
    for (int i = 0; i < NCU; i++) {
        OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_output[i]}, CL_MIGRATE_MEM_OBJECT_HOST));
    }
    OCL_CHECK(err, err = q.finish());

    bool match = verify(sw_results.data(), source_hw_results.data(), size);
    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
