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
#include <iomanip>
#include <vector>
#include <chrono>
#include "xcl2.hpp"

#include "experimental/xrt_device.h"
#include "experimental/xrt_bo.h"
#include "experimental/xrt_kernel.h"

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

    /* The command would incease */
    std::vector<unsigned int> cmds_per_run = {10,   50,   100,   200,   500,    1000,   1500,   2000,
                                              3000, 5000, 10000, 50000, 100000, 500000, 1000000};
    int expected_cmds = 10000;

    if (xcl::is_emulation()) {
        cmds_per_run = {10, 20};
        std::cout << "Number of operations is reduced for faster execution on "
                     "emulation flow.\n";
    }
    auto hello = xrt::kernel(device, uuid.get(), "hello");

    /* Create 'expected_cmds' commands if possible */
    std::vector<xrt::run> cmds;
    std::vector<xrt::bo> bos;
    for (int i = 0; i < expected_cmds; i++) {
        auto run = xrt::run(hello);
        auto bo = xrt::bo(device, 20, hello.group_id(0));
        run.set_arg(0, bo);
        cmds.push_back(std::move(run));
        bos.push_back(std::move(bo));
    }
    std::cout << "Allocated commands, expect " << expected_cmds << ", created " << cmds.size() << std::endl;

    for (auto num_cmds : cmds_per_run) {
        uint32_t i = 0;
        unsigned int issued = 0, completed = 0;
        auto start = std::chrono::high_resolution_clock::now();

        for (auto& cmd : cmds) {
            cmd.start();
            if (++issued == num_cmds) break;
        }

        while (completed < num_cmds) {
            cmds[i].wait();

            completed++;
            if (issued < num_cmds) {
                cmds[i].start();
                issued++;
            }

            if (++i == cmds.size()) i = 0;
        }

        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        std::cout << "Commands: " << std::setw(7) << num_cmds << " iops: " << (num_cmds * 1000.0 * 1000.0 / duration)
                  << std::endl;
    }
    std::cout << "TEST PASSED\n";
    return 0;
}
