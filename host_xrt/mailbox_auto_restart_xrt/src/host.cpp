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

// XRT includes
#include "experimental/xrt_mailbox.h"

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

    auto mbox_autorestart = xrt::kernel(device, uuid, "mbox_autorestart");

    uint32_t iter = 20;
    int adder1 = 20; // arbitrarily chosen to be different from 0
    int adder2 = 20; // arbitrarily chosen to be different from 0

    // start the kernel in auto restart mode with default scalar values
    auto mbox_autorestart_run = mbox_autorestart(xrt::autostart{0}, adder1, adder2);

    // create mailbox to programatically update the scalars adder1 and adder2
    xrt::mailbox krnl_mbox(mbox_autorestart_run);
    int hw_add = 0;
    int hw_mult = 0;
    bool match = true;

    for (unsigned int cnt = 0; cnt < iter; ++cnt) {
        std::cout << "iteration: " << cnt << " (adder1,adder2): " << adder1 << ',' << adder2 << ")\n";

        // prepare for next iteration, update the mailbox with the next
        // values of 'adder1' and 'adder2'.
        if (cnt % 2 == 0) {
            krnl_mbox.set_arg(0, ++adder1); // update the mailbox
            krnl_mbox.set_arg(1, --adder2); // update the mailbox

            // write the mailbox content to hw, the write will not be picked
            // up until the next iteration of the pipeline .
            krnl_mbox.write(); // requests sync of mailbox to hw
        } else {
            krnl_mbox.read(); // requests sync of mailbox to hw
            auto mbox_add = krnl_mbox.get_arg(2);
            auto mbox_mult = krnl_mbox.get_arg(3);
            hw_add = *((int*)mbox_add.first);
            hw_mult = *((int*)mbox_mult.first);
            auto sw_add = adder1 + adder2;
            auto sw_mult = adder1 * adder2;
            if (hw_add != sw_add or hw_mult != sw_mult) {
                std::cout << "HW_ADD : EXPECTED : " << sw_add << " OBSERVED : " << hw_add << std::endl;
                std::cout << "HW_MULT : EXPECTED : " << sw_mult << " OBSERVED : " << hw_mult << std::endl;
                match = false;
                break;
            }
        }
    }
    //
    mbox_autorestart_run.stop();
    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;
    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
