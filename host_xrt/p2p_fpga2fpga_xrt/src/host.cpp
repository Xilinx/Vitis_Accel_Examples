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

#include "cmdlineparser.h"
#include <iostream>
#include <cstring>
#include <iomanip>

// XRT includes
#include "experimental/xrt_bo.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_kernel.h"

#define DATA_SIZE 4096

int main(int argc, char** argv) {
    // Command Line Parser
    sda::utils::CmdLineParser parser;

    // Switches
    //**************//"<Full Arg>",  "<Short Arg>", "<Description>", "<Default>"
    parser.addSwitch("--xclbin_file", "-x", "input binary file string", "");
    parser.addSwitch("--device_id1", "-d1", "device index", "0");
    parser.addSwitch("--device_id2", "-d2", "device index", "1");
    parser.parse(argc, argv);

    // Read settings
    std::string binaryFile = parser.value("xclbin_file");
    int device_index1 = stoi(parser.value("device_id1"));
    int device_index2 = stoi(parser.value("device_id2"));

    if (argc < 3) {
        parser.printHelp();
        return EXIT_FAILURE;
    }

    std::cout << "Open the device1 : " << device_index1 << std::endl;
    auto device1 = xrt::device(device_index1);
    std::cout << "Load the xclbin " << binaryFile << std::endl;
    auto uuid = device1.load_xclbin(binaryFile);

    std::cout << "Open the device2 : " << device_index2 << std::endl;
    auto device2 = xrt::device(device_index2);

    auto nodma_cnt = 0;
    if (device1.get_info<xrt::info::device::nodma>()) nodma_cnt++;
    if (device2.get_info<xrt::info::device::nodma>()) nodma_cnt++;

    if (nodma_cnt == 2) {
        std::cout
            << "WARNING: P2P transfer can only be done between xdma and nodma devices but not between 2 nodma devices. "
               "Please run this "
               "design on machine with both xdma and nodma devices.\n";
        return 0;
    }

    size_t vector_size_bytes = sizeof(int) * DATA_SIZE;

    auto krnl = xrt::kernel(device1, uuid, "increment");

    xrt::bo::flags flags = xrt::bo::flags::p2p;
    std::cout << "Allocate Buffer in Global Memory\n";
    auto in1 = xrt::bo(device1, vector_size_bytes, krnl.group_id(0));
    // If p2p is not enabled, we need to uncomment line 65 and comment line 66.
    // auto out1 = xrt::bo(device1, vector_size_bytes, krnl.group_id(1));
    auto out1 = xrt::bo(device1, vector_size_bytes, flags, krnl.group_id(1));

    // Map the contents of the buffer object into host memory
    auto in1_map = in1.map<int*>();
    auto out1_map = out1.map<int*>();

    auto in2 = xrt::bo(device2, out1_map, vector_size_bytes, 0);

    // Create the test data
    int bufReference[DATA_SIZE];
    for (int i = 0; i < DATA_SIZE; ++i) {
        in1_map[i] = i;
        bufReference[i] = in1_map[i] + 10;
    }

    // Synchronize buffer content with device side
    std::cout << "synchronize input buffer data to device global memory\n";
    in1.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    std::cout << "Execution of the kernel\n";
    auto run = krnl(in1, out1, 10, DATA_SIZE);
    run.wait();

    // Get the output;
    std::cout << "Get the output data from the device" << std::endl;

    int loop = 10000;
    std::chrono::high_resolution_clock::time_point t_start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < loop; i++) {
        // The below line should be uncommented if you are running the design when p2p is not enabled.
        // out1.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
        in2.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    }
    std::chrono::high_resolution_clock::time_point t_end = std::chrono::high_resolution_clock::now();
    // Calculations
    unsigned long t_duration = std::chrono::duration_cast<std::chrono::microseconds>(t_end - t_start).count();
    double secduration = t_duration / (double)1000000;
    double gbpersec = ((loop * vector_size_bytes) / secduration) / ((double)1024 * 1024 * 1024);
    std::cout << "Bytes Transfer = " << vector_size_bytes << " Iterations = " << loop
              << "\nThroughput= " << std::setprecision(2) << std::fixed << gbpersec << "GB/s\n";

    // Validate our results
    if (std::memcmp(out1_map, bufReference, DATA_SIZE))
        throw std::runtime_error("Value read back does not match reference");

    std::cout << "TEST PASSED\n";
    return 0;
}
