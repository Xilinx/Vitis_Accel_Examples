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

/********************************************************************************************
 * Description:
 *
 * Xilinx's High Bandwidth Memory (HBM)-enabled FPGA are the clear solution for
 * providing
 * massive memory bandwidth within the lowest power, footprint, and system cost
 * envolopes.
 *
 * There are total 32 memory resources referenced as HBM[0:31] by V++ and each
 * has a
 * capacity of storing 256MB of data.
 *
 * This example showcases how to use buffers larger than 256MB (e.g. 1GB) using
 * pseudo-channel(PC) grouping.
 *
 *          +-----------+                   +-----------+
 *          |           |                   |           |
 *          | HBM[0:31] | ---- Input1 ----> |           |
 *          |           |                   |           |
 *          +-----------+                   |           |
 *                                          |           |
 *          +-----------+                   |           |
 *          |           |                   |   KERNEL  |
 *          | HBM[0:31] | ---- Input2 ----> |           |
 *          |           |                   |           |
 *          +-----------+                   |           |
 *                                          |           |
 *          +-----------+                   |           |
 *          |           |                   |           |
 *          | HBM[0:31] | <---- Output ---- |           |
 *          |           |                   |           |
 *          +-----------+                   +-----------+
 *
 *  *****************************************************************************************/

#include "xcl2.hpp"
#include <algorithm>
#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

// Function for verifying results
bool verify(std::vector<int, aligned_allocator<int> >& source_sw_results,
            std::vector<int, aligned_allocator<int> >& source_hw_results,
            unsigned int size) {
    bool check = true;
    for (size_t i = 0; i < size; i++) {
        if (source_hw_results[i] != source_sw_results[i]) {
            std::cout << "Error: Result mismatch" << std::endl;
            std::cout << "i = " << i << " CPU result = " << source_sw_results[i]
                      << " Device result = " << source_hw_results[i] << std::endl;
            check = false;
            // break;
        }
    }
    return check;
}

double run_krnl(cl::Context& context,
                cl::CommandQueue& q,
                cl::Kernel& kernel,
                std::vector<int, aligned_allocator<int> >& source_in1,
                std::vector<int, aligned_allocator<int> >& source_in2,
                std::vector<int, aligned_allocator<int> >& source_hw_results,
                unsigned int num_elements) {
    cl_int err;

    // These commands will allocate memory on the FPGA. The cl::Buffer objects can
    // be used to reference the memory locations on the device.
    // Creating Buffers
    OCL_CHECK(err, cl::Buffer buffer_input1(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                            sizeof(uint32_t) * num_elements, source_in1.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_input2(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                            sizeof(uint32_t) * num_elements, source_in2.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_output(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                                            sizeof(uint32_t) * num_elements, source_hw_results.data(), &err));

    // Setting the kernel Arguments
    OCL_CHECK(err, err = (kernel).setArg(0, buffer_input1));
    OCL_CHECK(err, err = (kernel).setArg(1, buffer_input2));
    OCL_CHECK(err, err = (kernel).setArg(2, buffer_output));
    OCL_CHECK(err, err = (kernel).setArg(3, num_elements));

    // Copy input data to Device Global Memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_input1, buffer_input2}, 0 /* 0 means from host*/));
    q.finish();

    std::chrono::duration<double> kernel_time(0);

    auto kernel_start = std::chrono::high_resolution_clock::now();
    OCL_CHECK(err, err = q.enqueueTask(kernel));
    q.finish();
    auto kernel_end = std::chrono::high_resolution_clock::now();

    kernel_time = std::chrono::duration<double>(kernel_end - kernel_start);

    // Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_output}, CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();

    return kernel_time.count();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <XCLBIN> \n", argv[0]);
        return -1;
    }
    cl_int err;
    cl::Context context;
    cl::CommandQueue q;
    cl::Kernel kernel_vadd;
    std::string binaryFile = argv[1];

    // The get_xil_devices will return vector of Xilinx Devices
    auto devices = xcl::get_xil_devices();

    // read_binary_file() command will find the OpenCL binary file created using
    // the
    // V++ compiler load into OpenCL Binary and return pointer to file buffer.
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
            OCL_CHECK(err, kernel_vadd = cl::Kernel(program, "krnl_vadd", &err));
            valid_device = true;
            break; // we break because we found a valid device
        }
    }
    if (!valid_device) {
        std::cout << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }

    unsigned int dataSize = 1024 * 1024 * 1024;
    if (xcl::is_emulation()) {
        dataSize = 4 * 1024;
        std::cout << "Original Dataset is reduced for faster execution on "
                     "emulation flow. Data size="
                  << dataSize << std::endl;
    }

    unsigned int num_elements = dataSize / sizeof(uint32_t);

    std::vector<int, aligned_allocator<int> > source_in1(num_elements);
    std::vector<int, aligned_allocator<int> > source_in2(num_elements);
    std::vector<int, aligned_allocator<int> > source_hw_results(num_elements);
    std::vector<int, aligned_allocator<int> > source_sw_results(num_elements);

    // Create the test data
    std::generate(source_in1.begin(), source_in1.end(), std::rand);
    std::generate(source_in2.begin(), source_in2.end(), std::rand);

    for (size_t i = 0; i < num_elements; i++) {
        source_sw_results[i] = source_in1[i] + source_in2[i];
        source_hw_results[i] = 0;
    }

    double kernel_time_in_sec = 0, result = 0;
    bool match = true;

    std::cout << "Running Three Separate Groups for Three Buffers" << std::endl;

    std::cout << "Each buffer is allocated with different HBM Group." << std::endl;
    std::cout << "input 1 -> PC 0 : 31 " << std::endl;
    std::cout << "input 2 -> PC 0 : 31 " << std::endl;
    std::cout << "output  -> PC 0 : 31 " << std::endl;

    kernel_time_in_sec = run_krnl(context, q, kernel_vadd, source_in1, source_in2, source_hw_results, num_elements);
    match = verify(source_sw_results, source_hw_results, num_elements);

    result = 3 * dataSize;
    result /= (1000 * 1000 * 1000); // to GB
    result /= kernel_time_in_sec;   // to GBps

    std::cout << "THROUGHPUT = " << result << " GB/s " << std::endl;

    std::cout << (match ? "TEST PASSED" : "TEST FAILED") << std::endl;
    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
