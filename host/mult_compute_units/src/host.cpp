/**
* Copyright (C) 2019-2021 Xilinx, Inc
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

/*This is simple Example of Multiple Compute units to showcase how a single
kernel
can be instantiated into Multiple compute units. Host code will show how to use
multiple compute units and run them concurrently. */
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <vector>

// OpenCL utility layer include
#include "xcl2.hpp"

auto constexpr data_hw = 1024 * 1024 * 4;
auto constexpr num_cu = 4;

//////////////MAIN FUNCTION//////////////
int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << "<XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }
    std::string binaryFile = argv[1];
    int data_size = data_hw;
    cl_int err;
    if (xcl::is_emulation()) {
        data_size = 4096 * 4;
    }
    cl::CommandQueue q;
    cl::Context context;
    std::vector<cl::Kernel> krnls(num_cu);

    // I/O data vectors
    std::vector<int, aligned_allocator<int> > source_in1(data_size);
    std::vector<int, aligned_allocator<int> > source_in2(data_size);
    std::vector<int, aligned_allocator<int> > source_hw_results(data_size);
    std::vector<int> sw_results(data_size);

    // Create test data
    std::generate(source_in1.begin(), source_in1.end(), std::rand);
    std::generate(source_in2.begin(), source_in2.end(), std::rand);
    for (int i = 0; i < data_size; i++) {
        sw_results[i] = source_in1[i] + source_in2[i];
    }

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
            // Creating Kernel objects
            for (int i = 0; i < num_cu; i++) {
                OCL_CHECK(err, krnls[i] = cl::Kernel(program, "vadd", &err));
            }
            valid_device = true;
            break; // we break because we found a valid device
        }
    }
    if (!valid_device) {
        std::cout << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }

    // Creating buffers
    auto chunk_size = data_size / num_cu;
    size_t vector_size_bytes = sizeof(int) * chunk_size;
    std::vector<cl::Buffer> buffer_in1(num_cu);
    std::vector<cl::Buffer> buffer_in2(num_cu);
    std::vector<cl::Buffer> buffer_output(num_cu);

    for (int i = 0; i < num_cu; i++) {
        OCL_CHECK(err, buffer_in1[i] = cl::Buffer(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, vector_size_bytes,
                                                  source_in1.data() + i * chunk_size, &err));
        OCL_CHECK(err, buffer_in2[i] = cl::Buffer(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, vector_size_bytes,
                                                  source_in2.data() + i * chunk_size, &err));
        OCL_CHECK(err,
                  buffer_output[i] = cl::Buffer(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, vector_size_bytes,
                                                source_hw_results.data() + i * chunk_size, &err));
    }

    for (int i = 0; i < num_cu; i++) {
        int narg = 0;

        // Setting kernel arguments
        OCL_CHECK(err, err = krnls[i].setArg(narg++, buffer_in1[i]));
        OCL_CHECK(err, err = krnls[i].setArg(narg++, buffer_in2[i]));
        OCL_CHECK(err, err = krnls[i].setArg(narg++, buffer_output[i]));
        OCL_CHECK(err, err = krnls[i].setArg(narg++, chunk_size));

        // Copy input data to device global memory
        OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_in1[i], buffer_in2[i]}, 0 /* 0 means from host*/));
    }
    OCL_CHECK(err, err = q.finish());

    for (int i = 0; i < num_cu; i++) {
        // Launch the kernel
        OCL_CHECK(err, err = q.enqueueTask(krnls[i]));
    }
    OCL_CHECK(err, err = q.finish());

    // Copy result from device global memory to host local memory
    for (int i = 0; i < num_cu; i++) {
        OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_output[i]}, CL_MIGRATE_MEM_OBJECT_HOST));
    }
    OCL_CHECK(err, err = q.finish());

    // Verify results
    bool match = std::equal(sw_results.begin(), sw_results.end(), source_hw_results.begin());
    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;

    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
