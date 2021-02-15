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

#define DATA_SIZE 4096

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

    size_t vector_size_bytes = sizeof(int) * DATA_SIZE;
    auto krnl = xrt::kernel(device, uuid, "increment:{increment_1}", xrt::kernel::cu_access_mode::exclusive);

    std::cout << "Allocate Buffer in Global Memory\n";
    auto bo0 = xrt::bo(device, vector_size_bytes, krnl.group_id(0));

    // Prepare the input data
    int buff_data[DATA_SIZE];
    for (auto i = 0; i < DATA_SIZE; ++i) {
        buff_data[i] = i;
    }

    bo0.write(buff_data);
    bo0.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    int read_data;
    int write_data = 7;

    // Write the input data using write_register
    auto write_offset = krnl.offset(2);
    std::cout << "The input write data : " << write_data << "\n";
    krnl.write_register(write_offset, write_data);

    std::cout << "Execution of the kernel\n";
    auto run = krnl(bo0, DATA_SIZE, write_data, read_data);
    run.wait();

    // Read the output using read_register
    auto read_offset = krnl.offset(3);
    read_data = krnl.read_register(read_offset);

    std::cout << "The output read data : " << read_data << "\n";
    std::cout << "TEST PASSED\n";

    return 0;
}
