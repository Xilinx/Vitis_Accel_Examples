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
 * This example is designed to show a simple use case to understand how to use
 * HBM memory.
 *
 * There are total 32 memory resources referenced as HBM[0:31] by V++ and each
 * has a
 * capacity of storing 256MB of data.
 *
 * This example showcases two use cases to differentiate how efficiently one can
 * use HBM banks.
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
 *  In this case only one HBM Bank, i.e. HBM0, has been used for both the input
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
 *  In this case three different HBM Banks, i.e. HBM1, HBM2 and HBM3, have been
 * used for input
 *  vectors and the processed output vector.
 *  The banks HBM1 & HBM2 are used for input vectors whereas HBM3 is used for
 *  processed output vector.
 *
 *  The use case highlights significant change in the data transfer throughput
 * (in terms of
 *  Gigabytes per second) when a single and multiple HBM banks are used for the
 *  same application.
 *
 *  *****************************************************************************************/
#include "cmdlineparser.h"
#include <iostream>
#include <cstring>

// XRT includes
#include "experimental/xrt_bo.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_kernel.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

double run_krnl(xrtDeviceHandle device, xrt::kernel& krnl, int* bank_assign, unsigned int size) {
    size_t vector_size_bytes = sizeof(uint32_t) * size;

    std::cout << "Allocate Buffer in Global Memory\n";
    auto bo0 = xrt::bo(device, vector_size_bytes, bank_assign[0]);
    auto bo1 = xrt::bo(device, vector_size_bytes, bank_assign[1]);
    auto bo_out = xrt::bo(device, vector_size_bytes, bank_assign[2]);

    // Map the contents of the buffer object into host memory
    auto bo0_map = bo0.map<int*>();
    auto bo1_map = bo1.map<int*>();
    auto bo_out_map = bo_out.map<int*>();
    std::fill(bo0_map, bo0_map + size, 0);
    std::fill(bo1_map, bo1_map + size, 0);
    std::fill(bo_out_map, bo_out_map + size, 0);

    // Create the test data
    int bufReference[size];
    for (uint32_t i = 0; i < size; ++i) {
        bo0_map[i] = i;
        bo1_map[i] = i;
        bufReference[i] = bo0_map[i] + bo1_map[i];
    }

    // Synchronize buffer content with device side
    std::cout << "synchronize input buffer data to device global memory\n";

    bo0.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    bo1.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    std::chrono::duration<double> kernel_time(0);

    std::cout << "Execution of the kernel\n";
    auto kernel_start = std::chrono::high_resolution_clock::now();
    auto run = krnl(bo0, bo1, bo_out, size);
    run.wait();
    auto kernel_end = std::chrono::high_resolution_clock::now();

    kernel_time = std::chrono::duration<double>(kernel_end - kernel_start);
    // Get the output;
    std::cout << "Get the output data from the device" << std::endl;
    bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

    // Validate our results
    if (std::memcmp(bo_out_map, bufReference, size))
        throw std::runtime_error("Value read back does not match reference");

    return kernel_time.count();
}

int main(int argc, char* argv[]) {
    // Command Line Parser
    sda::utils::CmdLineParser parser;

    // Switches
    //**************//"<Full Arg>",  "<Short Arg>", "<Description>", "<Default>"
    parser.addSwitch("--xclbin_file", "-x", "input binary file string", "");
    parser.addSwitch("--device_id", "-d", "device index", "0");
    parser.parse(argc, argv);

    // Read settings
    std::string binaryFile = parser.value("xclbin_file");
    int device_index = stoi(parser.value("device_id"));

    if (argc < 3) {
        parser.printHelp();
        return EXIT_FAILURE;
    }

    std::cout << "Open the device" << device_index << std::endl;
    auto device = xrt::device(device_index);
    std::cout << "Load the xclbin " << binaryFile << std::endl;
    auto uuid = device.load_xclbin(binaryFile);

    auto krnl = xrt::kernel(device, uuid, "krnl_vadd");

    unsigned int dataSize = 1024 * 1024;
    double kernel_time_in_sec = 0, result = 0;
    const int numBuf = 3; // Since three buffers are being used
    int bank_assign[numBuf];
    for (int j = 0; j < numBuf; j++) {
        bank_assign[j] = 0;
    }
    std::cout << "Running CASE 1  : Single HBM for all three Buffers " << std::endl;
    std::cout << "input 1 -> bank 0 " << std::endl;
    std::cout << "input 2 -> bank 0 " << std::endl;
    std::cout << "output  -> bank 0 " << std::endl;

    kernel_time_in_sec = run_krnl(device, krnl, bank_assign, dataSize);

    // Multiplying the actual data size by 3 because three buffers are being used.
    result = 3 * dataSize * sizeof(uint32_t);
    result /= (1000 * 1000 * 1000); // to GB
    result /= kernel_time_in_sec;   // to GBps

    std::cout << "[CASE 1] THROUGHPUT = " << result << " GB/s" << std::endl;

    for (int j = 0; j < numBuf; j++) {
        bank_assign[j] = j + 1;
    }

    std::cout << "Running CASE 2: Three Separate Banks for Three Buffers" << std::endl;
    std::cout << "input 1 -> bank 1 " << std::endl;
    std::cout << "input 2 -> bank 2 " << std::endl;
    std::cout << "output  -> bank 3 " << std::endl;

    kernel_time_in_sec = run_krnl(device, krnl, bank_assign, dataSize);

    result = 3 * dataSize * sizeof(uint32_t);
    result /= (1000 * 1000 * 1000); // to GB
    result /= kernel_time_in_sec;   // to GBps

    std::cout << "[CASE 2] THROUGHPUT = " << result << " GB/s " << std::endl;

    std::cout << "TEST PASSED" << std::endl;
    return 0;
}
