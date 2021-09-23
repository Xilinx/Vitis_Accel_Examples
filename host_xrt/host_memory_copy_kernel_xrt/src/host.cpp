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
    auto device_name = device.get_info<xrt::info::device::name>();
    char* xcl_mode = getenv("XCL_EMULATION_MODE");
    if ((xcl_mode != nullptr) && !strcmp(xcl_mode, "hw_emu")) {
        if (device_name.find("u250_gen3x16_xdma_3_1_202020") != std::string::npos) {
            std::cout << "[INFO]: The example is not supported for u250_gen3x16_xdma_3_1_202020 for hw_emu. Please try "
                         "other flows."
                      << '\n';
            return EXIT_SUCCESS;
        }
    }
    std::cout << "Load the xclbin " << binaryFile << std::endl;
    auto uuid = device.load_xclbin(binaryFile);

    // Compute the size of array in bytes
    uint32_t size_in_bytes = DATA_SIZE * sizeof(uint32_t);

    auto compute_krnl = xrt::kernel(device, uuid, "krnl_vadd");
    auto copy_krnl = xrt::kernel(device, uuid, "copy_kernel");

    xrt::bo::flags host_flags = xrt::bo::flags::host_only;
    xrt::bo::flags device_flags = xrt::bo::flags::device_only;

    std::cout << "Creating Buffers ...\n";
    auto hostonly_bo0 = xrt::bo(device, size_in_bytes, host_flags, copy_krnl.group_id(0));
    auto hostonly_bo1 = xrt::bo(device, size_in_bytes, host_flags, copy_krnl.group_id(0));
    auto hostonly_bo_out = xrt::bo(device, size_in_bytes, host_flags, copy_krnl.group_id(0));

    auto deviceonly_bo0 = xrt::bo(device, size_in_bytes, device_flags, compute_krnl.group_id(0));
    auto deviceonly_bo1 = xrt::bo(device, size_in_bytes, device_flags, compute_krnl.group_id(1));
    auto deviceonly_bo_out = xrt::bo(device, size_in_bytes, device_flags, compute_krnl.group_id(2));

    // Map the contents of the buffer object into host memory
    auto bo0_map = hostonly_bo0.map<int*>();
    auto bo1_map = hostonly_bo1.map<int*>();
    auto bo_out_map = hostonly_bo_out.map<int*>();

    std::fill(bo0_map, bo0_map + DATA_SIZE, 0);
    std::fill(bo1_map, bo1_map + DATA_SIZE, 0);
    std::fill(bo_out_map, bo_out_map + DATA_SIZE, 0);

    int direction = 0;

    for (int i = 0; i < DATA_SIZE; ++i) {
        bo0_map[i] = i + 1;
    }

    std::cout << "Starting copy kernel to copy data from host to device " << std::endl;
    auto cpy_run = copy_krnl(hostonly_bo0, deviceonly_bo0, DATA_SIZE, direction);
    cpy_run.wait();

    for (int i = 0; i < DATA_SIZE; ++i) {
        bo1_map[i] = (i + 1) * 2;
    }

    auto cpy_run1 = copy_krnl(hostonly_bo1, deviceonly_bo1, DATA_SIZE, direction);
    cpy_run1.wait();

    for (int i = 0; i < DATA_SIZE; ++i) {
        bo_out_map[i] = 0;
    }

    std::cout << "Starting Compute Kernel \n";
    auto compute_run = compute_krnl(deviceonly_bo0, deviceonly_bo1, deviceonly_bo_out, DATA_SIZE);
    compute_run.wait();

    direction = 1;

    std::cout << "Starting copy kernel to copy data from device to host " << std::endl;
    auto cpy_run2 = copy_krnl(hostonly_bo_out, deviceonly_bo_out, DATA_SIZE, direction);
    cpy_run2.wait();

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
