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
#include <cstring>
#include <iostream>

// XRT includes
#include "experimental/xrt_bo.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_kernel.h"

#define DATA_SIZE (4 * 1024)
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

    // Compute the size of array in bytes
    uint32_t size_in_bytes = DATA_SIZE * sizeof(uint32_t);

    auto krnl = xrt::kernel(device, uuid, "krnl_vadd");

    xrt::bo::flags host_flags = xrt::bo::flags::host_only;
    xrt::bo::flags device_flags = xrt::bo::flags::device_only;

    std::cout << "Creating Buffers ...\n";
    auto hostonly_bo0 = xrt::bo(device, size_in_bytes, host_flags, krnl.group_id(0));
    auto hostonly_bo1 = xrt::bo(device, size_in_bytes, host_flags, krnl.group_id(1));
    auto hostonly_bo_out = xrt::bo(device, size_in_bytes, host_flags, krnl.group_id(2));

    auto deviceonly_bo0 = xrt::bo(device, size_in_bytes, device_flags, krnl.group_id(0));
    auto deviceonly_bo1 = xrt::bo(device, size_in_bytes, device_flags, krnl.group_id(1));
    auto deviceonly_bo_out = xrt::bo(device, size_in_bytes, device_flags, krnl.group_id(2));

    // Map the contents of the buffer object into host memory
    auto bo0_map = hostonly_bo0.map<int*>();
    auto bo1_map = hostonly_bo1.map<int*>();
    auto bo_out_map = hostonly_bo_out.map<int*>();

    std::fill(bo0_map, bo0_map + DATA_SIZE, 0);
    std::fill(bo1_map, bo1_map + DATA_SIZE, 0);
    std::fill(bo_out_map, bo_out_map + DATA_SIZE, 0);

    // Create the test data
    for (int i = 0; i < DATA_SIZE; ++i) {
        bo0_map[i] = i + 1;
        bo1_map[i] = (i + 1) * 2;
        bo_out_map[i] = 0;
    }

    std::cout << "Copying date from host only buffer to device only buffer\n";
    deviceonly_bo0.copy(hostonly_bo0);
    deviceonly_bo1.copy(hostonly_bo1);

    std::cout << "Execution of the kernel\n";
    auto run = krnl(deviceonly_bo0, deviceonly_bo1, deviceonly_bo_out, DATA_SIZE);
    run.wait();

    std::cout << "Copying output date from device only buffer to host only buffer\n";
    hostonly_bo_out.copy(deviceonly_bo_out);

    // Compare the results of the Device to the simulation
    bool match = true;
    for (int i = 0; i < DATA_SIZE; i++) {
        if (bo_out_map[i] != (bo0_map[i] + bo1_map[i])) {
            std::cout << "Error: Result mismatch" << std::endl;
            std::cout << "i = " << i << " CPU result = " << bo0_map[i] + bo1_map[i]
                      << " Device result = " << bo_out_map[i] << std::endl;
            match = false;
            break;
        }
    }
    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;
    return 0;
}
