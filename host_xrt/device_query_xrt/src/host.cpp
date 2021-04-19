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

// XRT includes
#include "experimental/xrt_device.h"

int main(int argc, char** argv) {
    // Command Line Parser
    sda::utils::CmdLineParser parser;

    // Switches
    parser.addSwitch("--device_id", "-d", "device index", "0");
    parser.parse(argc, argv);

    // Read settings
    int device_index = stoi(parser.value("device_id"));

    auto device = xrt::device(device_index);
    std::cout << "Device " << device_index << " : " << device.get_info<xrt::info::device::name>() << "\n";

    std::cout << "  device bdf      : " << device.get_info<xrt::info::device::bdf>() << "\n";
    std::cout << "  device kdma     : " << device.get_info<xrt::info::device::kdma>() << "\n";
    std::cout << "  device max freq : " << device.get_info<xrt::info::device::max_clock_frequency_mhz>() << "\n";
    std::cout << "  device m2m      : " << std::boolalpha << device.get_info<xrt::info::device::m2m>() << std::dec
              << "\n";
    std::cout << "  device nodma    : " << std::boolalpha << device.get_info<xrt::info::device::nodma>() << std::dec
              << "\n";

    std::cout << "TEST PASSED\n";
    return 0;
}
