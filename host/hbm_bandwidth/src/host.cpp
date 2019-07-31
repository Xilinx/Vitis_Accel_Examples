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

/********************************************************************************************
 * Description:
 * This is host application to test HBM Full bandwidth.
 * Design contains 8 compute units of Kernel. Each compute unit has full access to all HBM 
 * memory (0 to 31). Host application allocate buffers into all 32 HBM Banks(16 Input buffers
 * and 16 output buffers). Host application runs all 8 compute units together and measure
 * the overall HBM bandwidth.
 *
 ******************************************************************************************/

#include <algorithm>
#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "xcl2.hpp"

#define NUM_KERNEL 8

//HBM Banks requirements
#define MAX_HBM_BANKCOUNT 32
#define BANK_NAME(n) n | XCL_MEM_TOPOLOGY
const int bank[MAX_HBM_BANKCOUNT] = {
    BANK_NAME(0),  BANK_NAME(1),  BANK_NAME(2),  BANK_NAME(3),  BANK_NAME(4),
    BANK_NAME(5),  BANK_NAME(6),  BANK_NAME(7),  BANK_NAME(8),  BANK_NAME(9),
    BANK_NAME(10), BANK_NAME(11), BANK_NAME(12), BANK_NAME(13), BANK_NAME(14),
    BANK_NAME(15), BANK_NAME(16), BANK_NAME(17), BANK_NAME(18), BANK_NAME(19),
    BANK_NAME(20), BANK_NAME(21), BANK_NAME(22), BANK_NAME(23), BANK_NAME(24),
    BANK_NAME(25), BANK_NAME(26), BANK_NAME(27), BANK_NAME(28), BANK_NAME(29),
    BANK_NAME(30), BANK_NAME(31)};

// Function for verifying results
bool verify(std::vector<int, aligned_allocator<int>> &source_sw_add_results,
            std::vector<int, aligned_allocator<int>> &source_sw_mul_results,
            std::vector<int, aligned_allocator<int>> &source_hw_add_results,
            std::vector<int, aligned_allocator<int>> &source_hw_mul_results,
            unsigned int size) {
    bool check = true;
    for (size_t i = 0; i < size; i++) {
        if (source_hw_add_results[i] != source_sw_add_results[i]) {
            std::cout << "Error: Result mismatch in Addition Operation"
                      << std::endl;
            std::cout << "i = " << i
                      << " CPU result = " << source_sw_add_results[i]
                      << " Device result = " << source_hw_add_results[i]
                      << std::endl;
            check = false;
            break;
        }
        if (source_hw_mul_results[i] != source_sw_mul_results[i]) {
            std::cout << "Error: Result mismatch in Multiplication Operation"
                      << std::endl;
            std::cout << "i = " << i
                      << " CPU result = " << source_sw_mul_results[i]
                      << " Device result = " << source_hw_mul_results[i]
                      << std::endl;
            check = false;
            break;
        }
    }
    return check;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <XCLBIN> \n", argv[0]);
        return -1;
    }

    unsigned int dataSize =
        64 * 1024 *
        1024; //taking maximum possible data size value for an HBM bank
    unsigned int num_times =
        1024; //num_times specify, number of times a kernel will execute the same operation. This is needed
    //to keep the kernel busy to test the actual bandwidth of all banks running concurrently.

    //reducing the test data capacity to run faster in emulation mode
    if (xcl::is_emulation()) {
        dataSize = 1024;
        num_times = 64;
    }

    std::string binaryFile = argv[1];
    cl_int err;

    std::vector<int, aligned_allocator<int>> source_in1(dataSize);
    std::vector<int, aligned_allocator<int>> source_in2(dataSize);
    std::vector<int, aligned_allocator<int>> source_sw_add_results(dataSize);
    std::vector<int, aligned_allocator<int>> source_sw_mul_results(dataSize);

    std::vector<int, aligned_allocator<int>> source_hw_add_results[NUM_KERNEL];
    std::vector<int, aligned_allocator<int>> source_hw_mul_results[NUM_KERNEL];

    for (int i = 0; i < NUM_KERNEL; i++) {
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

    // Initializing output vectors to zero
    for (size_t i = 0; i < NUM_KERNEL; i++) {
        std::fill(source_hw_add_results[i].begin(),
                  source_hw_add_results[i].end(),
                  0);
        std::fill(source_hw_mul_results[i].begin(),
                  source_hw_mul_results[i].end(),
                  0);
    }

    // OPENCL HOST CODE AREA START
    // The get_xil_devices will return vector of Xilinx Devices
    auto devices = xcl::get_xil_devices();
    auto device = devices[0];

    // Creating Context and Command Queue (Out of order) for selected Device
    OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(err,
              cl::CommandQueue q(context,
                                 device,
                                 CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE |
                                     CL_QUEUE_PROFILING_ENABLE,
                                 &err));

    std::string device_name = device.getInfo<CL_DEVICE_NAME>();
    std::cout << "Found Device=" << device_name.c_str() << std::endl;

    // read_binary_file() command will find the OpenCL binary file created using the
    // xocc compiler load into OpenCL Binary and return pointer to file buffer.
   auto fileBuf = xcl::read_binary_file(binaryFile);

   cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    devices.resize(1);
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));

    // Creating Kernel object using Compute unit names
    std::string krnl_name = "krnl_vaddmul";
    std::vector<cl::Kernel> krnls(NUM_KERNEL);

    for (int i = 0; i < NUM_KERNEL; i++) {
        std::string cu_id = std::to_string(i + 1);
        std::string krnl_name_full =
            krnl_name + ":{" + "krnl_vaddmul_" + cu_id + "}";

        printf("Creating a kernel [%s] for CU(%d)\n",
               krnl_name_full.c_str(),
               i + 1);

        //Here Kernel object is created by specifying kernel name along with compute unit.
        //For such case, this kernel object can only access the specific Compute unit

        OCL_CHECK(err,
                  krnls[i] = cl::Kernel(program, krnl_name_full.c_str(), &err));
    }

    std::vector<cl_mem_ext_ptr_t> inBufExt1(NUM_KERNEL);
    std::vector<cl_mem_ext_ptr_t> inBufExt2(NUM_KERNEL);
    std::vector<cl_mem_ext_ptr_t> outAddBufExt(NUM_KERNEL);
    std::vector<cl_mem_ext_ptr_t> outMulBufExt(NUM_KERNEL);

    std::vector<cl::Buffer> buffer_input1(NUM_KERNEL);
    std::vector<cl::Buffer> buffer_input2(NUM_KERNEL);
    std::vector<cl::Buffer> buffer_output_add(NUM_KERNEL);
    std::vector<cl::Buffer> buffer_output_mul(NUM_KERNEL);

    // For Allocating Buffer to specific Global Memory Bank, user has to use cl_mem_ext_ptr_t
    // and provide the Banks
    for (int i = 0; i < NUM_KERNEL; i++) {
        inBufExt1[i].obj = source_in1.data();
        inBufExt1[i].param = 0;
        inBufExt1[i].flags = bank[i * 4];

        inBufExt2[i].obj = source_in2.data();
        inBufExt2[i].param = 0;
        inBufExt2[i].flags = bank[(i * 4) + 1];

        outAddBufExt[i].obj = source_hw_add_results[i].data();
        outAddBufExt[i].param = 0;
        outAddBufExt[i].flags = bank[(i * 4) + 2];

        outMulBufExt[i].obj = source_hw_mul_results[i].data();
        outMulBufExt[i].param = 0;
        outMulBufExt[i].flags = bank[(i * 4) + 3];
    }

    // These commands will allocate memory on the FPGA. The cl::Buffer objects can
    // be used to reference the memory locations on the device.
    //Creating Buffers
    for (int i = 0; i < NUM_KERNEL; i++) {
        OCL_CHECK(err,
                  buffer_input1[i] =
                      cl::Buffer(context,
                                 CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX |
                                     CL_MEM_USE_HOST_PTR,
                                 sizeof(uint32_t) * dataSize,
                                 &inBufExt1[i],
                                 &err));
        OCL_CHECK(err,
                  buffer_input2[i] =
                      cl::Buffer(context,
                                 CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX |
                                     CL_MEM_USE_HOST_PTR,
                                 sizeof(uint32_t) * dataSize,
                                 &inBufExt2[i],
                                 &err));
        OCL_CHECK(err,
                  buffer_output_add[i] =
                      cl::Buffer(context,
                                 CL_MEM_WRITE_ONLY | CL_MEM_EXT_PTR_XILINX |
                                     CL_MEM_USE_HOST_PTR,
                                 sizeof(uint32_t) * dataSize,
                                 &outAddBufExt[i],
                                 &err));
        OCL_CHECK(err,
                  buffer_output_mul[i] =
                      cl::Buffer(context,
                                 CL_MEM_WRITE_ONLY | CL_MEM_EXT_PTR_XILINX |
                                     CL_MEM_USE_HOST_PTR,
                                 sizeof(uint32_t) * dataSize,
                                 &outMulBufExt[i],
                                 &err));
    }

    // Copy input data to Device Global Memory
    for (int i = 0; i < NUM_KERNEL; i++) {
        OCL_CHECK(err,
                  err = q.enqueueMigrateMemObjects(
                      {buffer_input1[i], buffer_input2[i]},
                      0 /* 0 means from host*/));
    }
    q.finish();

    double kernel_time_in_sec = 0, result = 0;

    std::chrono::duration<double> kernel_time(0);

    auto kernel_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_KERNEL; i++) {
        //Setting the k_vadd Arguments
        OCL_CHECK(err, err = krnls[i].setArg(0, buffer_input1[i]));
        OCL_CHECK(err, err = krnls[i].setArg(1, buffer_input2[i]));
        OCL_CHECK(err, err = krnls[i].setArg(2, buffer_output_add[i]));
        OCL_CHECK(err, err = krnls[i].setArg(3, buffer_output_mul[i]));
        OCL_CHECK(err, err = krnls[i].setArg(4, dataSize));
        OCL_CHECK(err, err = krnls[i].setArg(5, num_times));

        //Invoking the kernel
        OCL_CHECK(err, err = q.enqueueTask(krnls[i]));
    }
    q.finish();
    auto kernel_end = std::chrono::high_resolution_clock::now();

    kernel_time = std::chrono::duration<double>(kernel_end - kernel_start);

    kernel_time_in_sec = kernel_time.count();
    kernel_time_in_sec /= NUM_KERNEL;

    // Copy Result from Device Global Memory to Host Local Memory
    for (int i = 0; i < NUM_KERNEL; i++) {
        OCL_CHECK(err,
                  err = q.enqueueMigrateMemObjects(
                      {buffer_output_add[i], buffer_output_mul[i]},
                      CL_MIGRATE_MEM_OBJECT_HOST));
    }
    q.finish();

    bool match = true;

    for (int i = 0; i < NUM_KERNEL; i++) {
        match = verify(source_sw_add_results,
                       source_sw_mul_results,
                       source_hw_add_results[i],
                       source_hw_mul_results[i],
                       dataSize);
    }

    // Multiplying the actual data size by 4 because four buffers are being used.
    result = 4 * (float)dataSize * num_times * sizeof(uint32_t);
    result /= 1000;               // to KB
    result /= 1000;               // to MB
    result /= 1000;               // to GB
    result /= kernel_time_in_sec; // to GBps

    std::cout << "THROUGHPUT = " << result << " GB/s" << std::endl;
    //OPENCL HOST CODE AREA ENDS


    std::cout << (match ? "TEST PASSED" : "TEST FAILED") << std::endl;
    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
