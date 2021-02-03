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

/*This is simple Example of Multiple Compute units to showcase how a single
kernel
can be instantiated into Multiple compute units. Host code will show how to use
multiple compute units and run them concurrently. */
#include "cmdlineparser.h"
#include <iostream>
#include <cstring>

// XRT includes
#include "experimental/xrt_bo.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_kernel.h"

#define DATA_SIZE 1024 * 64
#define num_cu 4

//////////////MAIN FUNCTION//////////////
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

    auto chunk_size = DATA_SIZE / num_cu;
    size_t vector_size_bytes = sizeof(int) * chunk_size;

    auto krnl = xrt::kernel(device, uuid, "vadd");

    std::cout << "Allocate Buffer in Global Memory\n";
    xrt::bo bo0[num_cu];
    xrt::bo bo1[num_cu];
    xrt::bo bo_out[num_cu];

    for (int i = 0; i < num_cu; i++) {
        bo0[i] = xrt::bo(device, vector_size_bytes, krnl.group_id(0));
        bo1[i] = xrt::bo(device, vector_size_bytes, krnl.group_id(1));
        bo_out[i] = xrt::bo(device, vector_size_bytes, krnl.group_id(2));
    }

    int* bo0_map[num_cu] = {nullptr};
    int* bo1_map[num_cu] = {nullptr};
    int* bo_out_map[num_cu] = {nullptr};

    // Map the contents of the buffer object into host memory
    for (int i = 0; i < num_cu; i++) {
        bo0_map[i] = bo0[i].map<int*>();
        bo1_map[i] = bo1[i].map<int*>();
        bo_out_map[i] = bo_out[i].map<int*>();
    }

    // Create the test data
    int bufReference[num_cu][chunk_size];
    for (int i = 0; i < num_cu; i++) {
        for (int j = 0; j < chunk_size; ++j) {
            bo0_map[i][j] = j;
            bo1_map[i][j] = j;
            bo_out_map[i][j] = 0;
            bufReference[i][j] = bo0_map[i][j] + bo1_map[i][j];
        }
    }

    xrt::run run[num_cu];
    // Synchronize buffer content with device side
    std::cout << "synchronize input buffer data to device global memory\n";
    for (int i = 0; i < num_cu; i++) {
        bo0[i].sync(XCL_BO_SYNC_BO_TO_DEVICE);
        bo1[i].sync(XCL_BO_SYNC_BO_TO_DEVICE);
    }

    std::cout << "Execution of the kernel\n";
    for (int i = 0; i < num_cu; i++) {
        run[i] = krnl(bo0[i], bo1[i], bo_out[i], chunk_size);
    }

    // Wait for the kernel to complete
    for (int i = 0; i < num_cu; i++) {
        run[i].wait();
    }

    // Get the output;
    std::cout << "Get the output data from the device" << std::endl;
    for (int i = 0; i < num_cu; i++) {
        bo_out[i].sync(XCL_BO_SYNC_BO_FROM_DEVICE);
    }

    // Validate our results
    for (int i = 0; i < num_cu; i++) {
        if (std::memcmp(bo_out_map[i], bufReference[i], chunk_size))
            throw std::runtime_error("Value read back does not match reference");
    }
    std::cout << "TEST PASSED\n";
    return 0;
}
