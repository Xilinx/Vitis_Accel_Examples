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

/********************************************************************************************
 * Description:
 *
 * Xilinx's High Bandwidth Memory (HBM)-enabled FPGA are the clear solution for
 * providing
 * massive memory bandwidth within the lowest power, footprint, and system cost
 * envolopes.
 *
 * This example is designed to show a simple use case to understand how to use
 * HBM memory.
 *
 * There are total 32 memory resources referenced as HBM[0:31] by V++ and each
 * has a
 * capacity of storing 256MB of data.
 *
 * This example showcases two use cases to differentiate how efficiently one can
 * use HBM pseudo_channels(PC).
 *
 * CASE 1:
 *          +-----------+                   +-----------+
 *          |           | ---- Input1 ----> |           |
 *          |           |                   |           |
 *          |   HBM0    | ---- Input2 ----> |   KERNEL  |
 *          |           |                   |           |
 *          |           | <---- Output ---- |           |
 *          +-----------+                   +-----------+
 *
 *  In this case only one HBM PC, i.e. HBM0, has been used for both the input
 *  vectors and the processed output vector.
 *
 *  CASE 2:
 *          +-----------+                   +-----------+
 *          |           |                   |           |
 *          |   HBM1    | ---- Input1 ----> |           |
 *          |           |                   |           |
 *          +-----------+                   |           |
 *                                          |           |
 *          +-----------+                   |           |
 *          |           |                   |   KERNEL  |
 *          |   HBM2    | ---- Input2 ----> |           |
 *          |           |                   |           |
 *          +-----------+                   |           |
 *                                          |           |
 *          +-----------+                   |           |
 *          |           |                   |           |
 *          |   HBM3    | <---- Output ---- |           |
 *          |           |                   |           |
 *          +-----------+                   +-----------+
 *
 *  In this case three different HBM PCs, i.e. HBM1, HBM2 and HBM3, have been
 * used for input
 *  vectors and the processed output vector.
 *  The PCs HBM1 & HBM2 are used for input vectors whereas HBM3 is used for
 *  processed output vector.
 *
 *  The use case highlights significant change in the data transfer throughput
 * (in terms of
 *  Gigabytes per second) when a single and multiple HBM PCs are used for the
 *  same application.
 *
 *  *****************************************************************************************/

#include "xcl2.hpp"
#include <algorithm>
#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

// Number of HBM PCs required
#define MAX_HBM_PC_COUNT 32
#define PC_NAME(n) n | XCL_MEM_TOPOLOGY
const int pc[MAX_HBM_PC_COUNT] = {
    PC_NAME(0),  PC_NAME(1),  PC_NAME(2),  PC_NAME(3),  PC_NAME(4),  PC_NAME(5),  PC_NAME(6),  PC_NAME(7),
    PC_NAME(8),  PC_NAME(9),  PC_NAME(10), PC_NAME(11), PC_NAME(12), PC_NAME(13), PC_NAME(14), PC_NAME(15),
    PC_NAME(16), PC_NAME(17), PC_NAME(18), PC_NAME(19), PC_NAME(20), PC_NAME(21), PC_NAME(22), PC_NAME(23),
    PC_NAME(24), PC_NAME(25), PC_NAME(26), PC_NAME(27), PC_NAME(28), PC_NAME(29), PC_NAME(30), PC_NAME(31)};

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
            break;
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
                int* pc_assign,
                unsigned int size) {
    cl_int err;

    // For Allocating Buffer to specific Global Memory PC, user has to use
    // cl_mem_ext_ptr_t
    // and provide the PCs
    cl_mem_ext_ptr_t inBufExt1, inBufExt2, outBufExt;

    inBufExt1.obj = source_in1.data();
    inBufExt1.param = 0;
    inBufExt1.flags = pc_assign[0];

    inBufExt2.obj = source_in2.data();
    inBufExt2.param = 0;
    inBufExt2.flags = pc_assign[1];

    outBufExt.obj = source_hw_results.data();
    outBufExt.param = 0;
    outBufExt.flags = pc_assign[2];

    // These commands will allocate memory on the FPGA. The cl::Buffer objects can
    // be used to reference the memory locations on the device.
    // Creating Buffers
    OCL_CHECK(err, cl::Buffer buffer_input1(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            sizeof(uint32_t) * size, &inBufExt1, &err));
    OCL_CHECK(err, cl::Buffer buffer_input2(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            sizeof(uint32_t) * size, &inBufExt2, &err));
    OCL_CHECK(err, cl::Buffer buffer_output(context, CL_MEM_WRITE_ONLY | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                                            sizeof(uint32_t) * size, &outBufExt, &err));

    // Setting the kernel Arguments
    OCL_CHECK(err, err = (kernel).setArg(0, buffer_input1));
    OCL_CHECK(err, err = (kernel).setArg(1, buffer_input2));
    OCL_CHECK(err, err = (kernel).setArg(2, buffer_output));
    OCL_CHECK(err, err = (kernel).setArg(3, size));

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

    unsigned int dataSize = 1024 * 1024;

    std::vector<int, aligned_allocator<int> > source_in1(dataSize);
    std::vector<int, aligned_allocator<int> > source_in2(dataSize);
    std::vector<int, aligned_allocator<int> > source_hw_results(dataSize);
    std::vector<int, aligned_allocator<int> > source_sw_results(dataSize);

    // Create the test data
    std::generate(source_in1.begin(), source_in1.end(), std::rand);
    std::generate(source_in2.begin(), source_in2.end(), std::rand);

    for (size_t i = 0; i < dataSize; i++) {
        source_sw_results[i] = source_in1[i] + source_in2[i];
        source_hw_results[i] = 0;
    }

    double kernel_time_in_sec = 0, result = 0;
    bool match = true;
    const int numBuf = 3; // Since three buffers are being used
    int pc_assign[numBuf];

    std::cout << "Running CASE 1  : Single HBM for all three Buffers " << std::endl;

    std::cout << "Each buffer is allocated with same HBM PC." << std::endl;
    std::cout << "input 1 -> PC 0 " << std::endl;
    std::cout << "input 2 -> PC 0 " << std::endl;
    std::cout << "output  -> PC 0 " << std::endl;
    for (int j = 0; j < numBuf; j++) {
        pc_assign[j] = pc[0];
    }

    kernel_time_in_sec =
        run_krnl(context, q, kernel_vadd, source_in1, source_in2, source_hw_results, pc_assign, dataSize);
    match = verify(source_sw_results, source_hw_results, dataSize);

    // Multiplying the actual data size by 3 because three buffers are being used.
    result = 3 * dataSize * sizeof(uint32_t);
    result /= (1000 * 1000 * 1000); // to GB
    result /= kernel_time_in_sec;   // to GBps

    std::cout << "[CASE 1] THROUGHPUT = " << result << " GB/s" << std::endl;

    std::cout << "Running CASE 2: Three Separate PCs for Three Buffers" << std::endl;

    std::cout << "Each buffer is allocated with different HBM PCs." << std::endl;
    std::cout << "input 1 -> PC 1 " << std::endl;
    std::cout << "input 2 -> PC 2 " << std::endl;
    std::cout << "output  -> PC 3 " << std::endl;
    for (int j = 0; j < numBuf; j++) {
        pc_assign[j] = pc[j + 1];
    }

    kernel_time_in_sec =
        run_krnl(context, q, kernel_vadd, source_in1, source_in2, source_hw_results, pc_assign, dataSize);
    match = verify(source_sw_results, source_hw_results, dataSize);

    result = 3 * dataSize * sizeof(uint32_t);
    result /= (1000 * 1000 * 1000); // to GB
    result /= kernel_time_in_sec;   // to GBps

    std::cout << "[CASE 2] THROUGHPUT = " << result << " GB/s " << std::endl;

    std::cout << (match ? "TEST PASSED" : "TEST FAILED") << std::endl;
    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
