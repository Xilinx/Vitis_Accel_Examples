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
#include <cstring>

// XRT includes
#include "experimental/xrt_bo.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_kernel.h"

#define DATA_SIZE 256

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

    size_t size_in_bytes = sizeof(int) * DATA_SIZE;

    auto krnl = xrt::kernel(device, uuid, "dummy_kernel");

    std::cout << "Allocate Buffer in Global Memory\n";
    auto input_buffer = xrt::bo(device, size_in_bytes, krnl.group_id(0));
    auto output_buffer = xrt::bo(device, size_in_bytes, krnl.group_id(1));

    // Prepare the input data
    int buff_in_data[DATA_SIZE], buff_out_data[DATA_SIZE];
    for (auto i = 0; i < DATA_SIZE; ++i) {
        buff_in_data[i] = i;
        buff_out_data[i] = 0;
    }

    std::cout << "Write the input data\n";
    input_buffer.write(buff_in_data);

    std::cout << "synchronize input buffer data to device global memory\n";
    input_buffer.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    std::cout << "Execution of the kernel\n";
    auto run = krnl(input_buffer, output_buffer, DATA_SIZE);
    run.wait();

    std::cout << "Synchronize the output buffer data from the device" << std::endl;
    output_buffer.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

    std::cout << "Read the output data\n";
    output_buffer.read(buff_out_data);

    // Validate our results
    if (std::memcmp(buff_in_data, buff_out_data, DATA_SIZE))
        throw std::runtime_error("Value read back does not match reference");

    std::cout << "TEST PASSED\n";
    return 0;
}
