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

#include "cmdlineparser.h"
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <vector>
#include <cstring>

// XRT includes
#include "experimental/xrt_bo.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_kernel.h"
const int MAX_DIM = 16;

int main(int argc, char** argv) {
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
    const size_t array_size = MAX_DIM * MAX_DIM;
    const size_t size_in_bytes = array_size * sizeof(int);

    auto krnl_madd = xrt::kernel(device, uuid.get(), "madd");
    auto krnl_mmult = xrt::kernel(device, uuid.get(), "mmult");

    xrt::bo::flags flags = xrt::bo::flags::device_only;
    std::cout << "Allocate Buffer in Global Memory\n";
    auto bo0 = xrt::bo(device, size_in_bytes, krnl_mmult.group_id(0));
    auto bo1 = xrt::bo(device, size_in_bytes, krnl_mmult.group_id(1));
    auto bo_dev_only = xrt::bo(device, size_in_bytes, flags, krnl_mmult.group_id(2));

    auto bo2 = xrt::bo(device, size_in_bytes, krnl_madd.group_id(1));
    auto bo_out = xrt::bo(device, size_in_bytes, krnl_madd.group_id(2));

    // Map the contents of the buffer object into host memory
    auto bo0_map = bo0.map<int*>();
    auto bo1_map = bo1.map<int*>();
    auto bo2_map = bo2.map<int*>();
    auto bo_out_map = bo_out.map<int*>();
    std::fill(bo0_map, bo0_map + array_size, 0);
    std::fill(bo1_map, bo1_map + array_size, 0);
    std::fill(bo2_map, bo2_map + array_size, 0);
    std::fill(bo_out_map, bo_out_map + array_size, 0);

    // Create the test data
    int bufReference[array_size] = {0};
    for (int i = 0; i < (int)array_size; ++i) {
        bo0_map[i] = i;
        bo1_map[i] = 2 * i;
        bo2_map[i] = 1;
    }

    // generating sw_results
    for (int i = 0; i < MAX_DIM; i++) {
        for (int j = 0; j < MAX_DIM; j++) {
            for (int k = 0; k < MAX_DIM; k++) {
                bufReference[i * MAX_DIM + j] += bo0_map[i * MAX_DIM + k] * bo1_map[k * MAX_DIM + j];
            }
        }
    }

    for (int i = 0; i < MAX_DIM * MAX_DIM; i++) bufReference[i] += bo2_map[i];

    // Synchronize buffer content with device side
    std::cout << "synchronize input buffer data to device global memory\n";
    bo0.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    bo1.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    bo2.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    std::cout << "Execution of the mmult kernel\n";
    auto run = krnl_mmult(bo0, bo1, bo_dev_only, MAX_DIM);
    run.wait();

    std::cout << "Execution of the madd kernel\n";
    auto run1 = krnl_madd(bo_dev_only, bo2, bo_out, MAX_DIM);
    run1.wait();

    // Get the output;
    std::cout << "Get the output data from the device" << std::endl;
    bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

    // Validate our results
    if (std::memcmp(bo_out_map, bufReference, array_size))
        throw std::runtime_error("Value read back does not match reference");

    std::cout << "TEST PASSED\n";
    return 0;
}
