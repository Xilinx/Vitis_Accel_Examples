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
#include "experimental/xrt_bo.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_kernel.h"

static const int DATA_SIZE = 1024;
static const std::string error_message =
    "Error: Result mismatch:\n"
    "i = %d CPU result = %d Device result = %d\n";

// This example illustrates the simple OpenCL example that performs
// buffer copy from one buffer to another
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
    auto device_name = device.get_info<xrt::info::device::name>();
    char* xcl_mode = getenv("XCL_EMULATION_MODE");
    if ((xcl_mode != nullptr) && !strcmp(xcl_mode, "hw_emu")) {
        if (device_name.find("2018") != std::string::npos) {
            std::cout << "[INFO]: The example is not supported for " << device_name
                      << " for hw_emu. Please try other flows." << '\n';
            return EXIT_SUCCESS;
        }
    }
    if (device_name.find("u50_gen3x16_xdma_2019") != std::string::npos) {
        std::cout << "[INFO]: The example runs on non-CDMA mode for " << device_name << '\n';
    }
    std::cout << "Load the xclbin " << binaryFile << std::endl;
    auto uuid = device.load_xclbin(binaryFile);

    size_t size_in_bytes = sizeof(int) * DATA_SIZE;

    auto krnl = xrt::kernel(device, uuid, "vector_add");

    std::cout << "Allocate Buffer in Global Memory\n";
    auto bo_a = xrt::bo(device, size_in_bytes, krnl.group_id(0));
    auto bo_b = xrt::bo(device, size_in_bytes, krnl.group_id(1));
    auto bo_out = xrt::bo(device, size_in_bytes, krnl.group_id(2));

    // Map the contents of the buffer object into host memory
    auto bo_a_map = bo_a.map<int*>();
    auto bo_b_map = bo_b.map<int*>();
    auto bo_out_map = bo_out.map<int*>();
    std::fill(bo_a_map, bo_a_map + DATA_SIZE, 0);
    std::fill(bo_b_map, bo_b_map + DATA_SIZE, 0);
    std::fill(bo_out_map, bo_out_map + DATA_SIZE, 0);

    // Create the test data
    int bufReference[DATA_SIZE];
    for (int i = 0; i < DATA_SIZE; ++i) {
        bo_a_map[i] = 13;
        bo_b_map[i] = 0;
        bufReference[i] = bo_a_map[i] + bo_a_map[i];
    }

    // Synchronize buffer content with device side
    std::cout << "synchronize input buffer data to device global memory\n";
    bo_a.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    std::cout << "Copying date from buffer a to buffer b\n";
    bo_b.copy(bo_a);
    std::cout << "Execution of the kernel\n";
    auto run = krnl(bo_out, bo_a, bo_b, DATA_SIZE);
    run.wait();

    // Get the output;
    std::cout << "Get the output data from the device" << std::endl;
    bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

    // Validate our results
    if (std::memcmp(bo_out_map, bufReference, DATA_SIZE))
        throw std::runtime_error("Value read back does not match reference");

    std::cout << "TEST PASSED\n";
    return 0;
}
