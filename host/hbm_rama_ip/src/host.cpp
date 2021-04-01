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
Description:
    This is a HBM bandwidth example using a pseudo random 1024 bit data access
pattern to mimic Ethereum Ethash workloads. The design contains 2 compute units
of a kernel, reading 1024 bits from a pseudo random address in each of 2 pseudo
channels and writing the results of a simple mathematical operation to a pseudo
random address in 2 other pseudo channels. To maximize bandwidth the pseudo
channels are used in  P2P like configuration.
    Each buffer is 1 GB large requiring 4 HBM banks. Since each CU requires 4
buffers, the .cfg file is allocating the buffers to all the 32 HBM banks. The
host application runs the compute units concurrently to measure the overall
bandwidth between kernel and HBM Memory. For applications requiring random data
access, performance can vary significantly depending on the application
requirements (for example, the ratio of read and write operations, minimum data
word size, and memory size). The RAMA IP addresses such problems by
significantly improving memory access efficiency in cases where the required
memory exceeds 256 MB (one HBM pseudo-channel).
 ******************************************************************************************/

#include <algorithm>
#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "xcl2.hpp"

#define NUM_KERNEL 4

// Function for verifying results
bool verify(std::vector<uint32_t, aligned_allocator<uint32_t> >& source_sw_add_results,
            std::vector<uint32_t, aligned_allocator<uint32_t> >& source_sw_mul_results,
            std::vector<uint32_t, aligned_allocator<uint32_t> >& source_hw_add_results,
            std::vector<uint32_t, aligned_allocator<uint32_t> >& source_hw_mul_results,
            unsigned int size) {
    bool check = true;
    for (size_t i = 0; i < size; i++) {
        if (source_hw_add_results[i] != source_sw_add_results[i]) {
            std::cout << "Error: Result mismatch in Addition Operation" << std::endl;
            std::cout << "i = " << i << " CPU result = " << source_sw_add_results[i]
                      << " Device result = " << source_hw_add_results[i] << std::endl;
            check = false;
            break;
        }
        if (source_hw_mul_results[i] != source_sw_mul_results[i]) {
            std::cout << "Error: Result mismatch in Multiplication Operation" << std::endl;
            std::cout << "i = " << i << " CPU result = " << source_sw_mul_results[i]
                      << " Device result = " << source_hw_mul_results[i] << std::endl;
            check = false;
            break;
        }
    }
    return check;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << "<XCLBIN> \n";
        return -1;
    }

    unsigned int dataSize = 256 * 1024 * 1024; // taking maximum possible data size value for an HBM bank
    unsigned int num_times = 1024;             // num_times specify, number of times a kernel
                                               // will execute the same operation. This is
                                               // needed
    // to keep the kernel busy to test the actual bandwidth of all banks running
    // concurrently.

    // reducing the test data capacity to run faster in emulation mode
    if (xcl::is_emulation()) {
        dataSize = 1024;
        num_times = 64;
    }

    std::string binaryFile = argv[1];
    cl_int err;
    cl::CommandQueue q;
    std::string krnl_name = "krnl_vaddmul";
    std::vector<cl::Kernel> krnls(NUM_KERNEL);
    int NUM_BUFFER = NUM_KERNEL / 2;
    cl::Context context;
    std::vector<uint32_t, aligned_allocator<uint32_t> > source_in1(dataSize);
    std::vector<uint32_t, aligned_allocator<uint32_t> > source_in2(dataSize);
    std::vector<uint32_t, aligned_allocator<uint32_t> > source_sw_add_results(dataSize);
    std::vector<uint32_t, aligned_allocator<uint32_t> > source_sw_mul_results(dataSize);

    std::vector<uint32_t, aligned_allocator<uint32_t> > source_hw_add_results[NUM_BUFFER];
    std::vector<uint32_t, aligned_allocator<uint32_t> > source_hw_mul_results[NUM_BUFFER];

    for (int i = 0; i < NUM_BUFFER; i++) {
        source_hw_add_results[i].resize(dataSize);
        source_hw_mul_results[i].resize(dataSize);
    }

    // Create the test data
    std::generate(source_in1.begin(), source_in1.end(), std::rand);
    std::generate(source_in2.begin(), source_in2.end(), std::rand);
    for (size_t i = 0; i < dataSize; i++) {
        source_sw_add_results[i] = source_in1[i] + source_in2[i];
        source_sw_mul_results[i] = source_in1[i] * source_in2[i];
    }

    // OPENCL HOST CODE AREA START
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
        OCL_CHECK(err, q = cl::CommandQueue(context, device,
                                            CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | CL_QUEUE_PROFILING_ENABLE, &err));

        std::cout << "Trying to program device[" << i << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
        cl::Program program(context, {device}, bins, nullptr, &err);
        if (err != CL_SUCCESS) {
            std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
        } else {
            std::cout << "Device[" << i << "]: program successful!\n";
            // Creating Kernel object using Compute unit names

            for (int i = 0; i < NUM_KERNEL; i++) {
                std::string cu_id = std::to_string(i + 1);
                std::string krnl_name_full = krnl_name + ":{" + "krnl_vaddmul_" + cu_id + "}";

                std::cout << "Creating a kernel [" << krnl_name_full.c_str() << "] for CU(" << i + 1 << ")\n";

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
    if (valid_device == false) {
        std::cout << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }

    std::vector<cl::Buffer> buffer_input1(NUM_BUFFER);
    std::vector<cl::Buffer> buffer_input2(NUM_BUFFER);
    std::vector<cl::Buffer> buffer_output_add(NUM_BUFFER);
    std::vector<cl::Buffer> buffer_output_mul(NUM_BUFFER);

    // These commands will allocate memory on the FPGA. The cl::Buffer objects can
    // be used to reference the memory locations on the device.
    // Creating Buffers
    for (int i = 0; i < NUM_BUFFER; i++) {
        OCL_CHECK(err, buffer_input1[i] = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                                                     sizeof(uint32_t) * dataSize, source_in1.data(), &err));
        OCL_CHECK(err, buffer_input2[i] = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                                                     sizeof(uint32_t) * dataSize, source_in2.data(), &err));
        OCL_CHECK(
            err, buffer_output_add[i] = cl::Buffer(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
                                                   sizeof(uint32_t) * dataSize, source_hw_add_results[i].data(), &err));
        OCL_CHECK(
            err, buffer_output_mul[i] = cl::Buffer(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
                                                   sizeof(uint32_t) * dataSize, source_hw_mul_results[i].data(), &err));
    }

    // Copy input data to Device Global Memory
    for (int i = 0; i < NUM_KERNEL; i++) {
        // Setting the k_vadd Arguments
        int j = i % NUM_BUFFER;
        OCL_CHECK(err, err = krnls[i].setArg(0, buffer_input1[j]));
        OCL_CHECK(err, err = krnls[i].setArg(1, buffer_input2[j]));
        OCL_CHECK(err, err = krnls[i].setArg(2, buffer_output_add[j]));
        OCL_CHECK(err, err = krnls[i].setArg(3, buffer_output_mul[j]));
        OCL_CHECK(err, err = krnls[i].setArg(4, dataSize));
        OCL_CHECK(err, err = krnls[i].setArg(5, num_times));
    }

    for (int i = 0; i < NUM_BUFFER; i++) {
        OCL_CHECK(err,
                  err = q.enqueueMigrateMemObjects({buffer_input1[i], buffer_input2[i]}, 0 /* 0 means from host*/));
    }
    q.finish();

    double kernel_time_in_sec_with_rama_ip = 0, result_with_rama_ip = 0;

    std::chrono::duration<double> kernel_time_with_rama_ip(0);

    auto kernel_start_with_rama_ip = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_BUFFER; i++) {
        // Invoking the kernel
        OCL_CHECK(err, err = q.enqueueTask(krnls[i]));
    }
    q.finish();
    auto kernel_end_with_rama_ip = std::chrono::high_resolution_clock::now();

    kernel_time_with_rama_ip = std::chrono::duration<double>(kernel_end_with_rama_ip - kernel_start_with_rama_ip);

    kernel_time_in_sec_with_rama_ip = kernel_time_with_rama_ip.count();
    kernel_time_in_sec_with_rama_ip /= NUM_BUFFER;

    // Copy Result from Device Global Memory to Host Local Memory
    for (int i = 0; i < NUM_BUFFER; i++) {
        OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_output_add[i], buffer_output_mul[i]},
                                                        CL_MIGRATE_MEM_OBJECT_HOST));
    }
    q.finish();

    bool match_with_rama_ip = true;

    for (int i = 0; i < NUM_BUFFER; i++) {
        match_with_rama_ip = verify(source_sw_add_results, source_sw_mul_results, source_hw_add_results[i],
                                    source_hw_mul_results[i], dataSize);
        if (!match_with_rama_ip) {
            std::cerr << "TEST FAILED" << std::endl;
            return EXIT_FAILURE;
        }
    }

    // Multiplying the actual data size by 4 because four buffers are being
    // used.
    result_with_rama_ip = 4 * (float)dataSize * num_times * sizeof(uint32_t);
    result_with_rama_ip /= 1000;                            // to KB
    result_with_rama_ip /= 1000;                            // to MB
    result_with_rama_ip /= 1000;                            // to GB
    result_with_rama_ip /= kernel_time_in_sec_with_rama_ip; // to GBps

    std::cout << "WITH RAMA IP -" << std::endl;
    std::cout << "OVERALL THROUGHPUT = " << result_with_rama_ip << " GB/s" << std::endl;
    std::cout << "CHANNEL THROUGHPUT = " << result_with_rama_ip / (NUM_BUFFER * 4) << " GB/s" << std::endl;

    double kernel_time_in_sec_without_rama_ip = 0, result_without_rama_ip = 0;

    std::chrono::duration<double> kernel_time_without_rama_ip(0);

    auto kernel_start_without_rama_ip = std::chrono::high_resolution_clock::now();
    for (int i = NUM_BUFFER; i < NUM_KERNEL; i++) {
        // Invoking the kernel
        OCL_CHECK(err, err = q.enqueueTask(krnls[i]));
    }
    q.finish();
    auto kernel_end_without_rama_ip = std::chrono::high_resolution_clock::now();

    kernel_time_without_rama_ip =
        std::chrono::duration<double>(kernel_end_without_rama_ip - kernel_start_without_rama_ip);

    kernel_time_in_sec_without_rama_ip = kernel_time_without_rama_ip.count();
    kernel_time_in_sec_without_rama_ip /= NUM_BUFFER;

    // Copy Result from Device Global Memory to Host Local Memory
    for (int i = 0; i < NUM_BUFFER; i++) {
        OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_output_add[i], buffer_output_mul[i]},
                                                        CL_MIGRATE_MEM_OBJECT_HOST));
    }
    q.finish();

    bool match_without_rama_ip = true;

    for (int i = 0; i < NUM_BUFFER; i++) {
        match_without_rama_ip = verify(source_sw_add_results, source_sw_mul_results, source_hw_add_results[i],
                                       source_hw_mul_results[i], dataSize);
        if (!match_without_rama_ip) {
            std::cerr << "TEST FAILED" << std::endl;
            return EXIT_FAILURE;
        }
    }

    // Multiplying the actual data size by 4 because four buffers are being
    // used.
    result_without_rama_ip = 4 * (float)dataSize * num_times * sizeof(uint32_t);
    result_without_rama_ip /= 1000;                               // to KB
    result_without_rama_ip /= 1000;                               // to MB
    result_without_rama_ip /= 1000;                               // to GB
    result_without_rama_ip /= kernel_time_in_sec_without_rama_ip; // to GBps

    std::cout << "WITHOUT RAMA IP -" << std::endl;
    std::cout << "OVERALL THROUGHPUT = " << result_without_rama_ip << " GB/s" << std::endl;
    std::cout << "CHANNEL THROUGHPUT = " << result_without_rama_ip / (NUM_BUFFER * 4) << " GB/s" << std::endl;

    std::cout << "TEST PASSED" << std::endl;
    return EXIT_SUCCESS;
}
