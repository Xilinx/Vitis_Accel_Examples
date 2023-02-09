/**
 * Copyright (C) 2022 Xilinx, Inc
 * Copyright (C) 2022-2023 Advanced Micro Devices, Inc.
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

#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
#include <unistd.h>

// XRT Headers
#include "experimental/xrt_bo.h"
#include "experimental/xrt_device.h"
#include "experimental/xrt_kernel.h"

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>"
                  << " <Python Demo> " << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];
    auto device = xrt::device(0);
    auto uuid = device.load_xclbin(binaryFile);
    auto krnl_config = xrt::kernel(device, uuid, "exerciser_config_rx");
    xrt::bo::flags flags = xrt::bo::flags::host_only;
    
    // configuration
    int n_table_idx = 10;       // Number of indexes in table to be updated
    uint16_t udp_dest = 0x1234; // udp dest port address

    auto buffer_in_tables = xrt::bo(device, sizeof(uint16_t) * n_table_idx, flags, krnl_config.group_id(2));
    auto in_table = buffer_in_tables.map<uint16_t*>();
    
    // Assign table data
    for (int i = 0; i < n_table_idx; i++) {
        in_table[i] = i + 256;
    }

    // Execute Kernel
    auto run = krnl_config(n_table_idx, udp_dest, buffer_in_tables);    
    run.wait();

    std::cout << "Start of python process" << std::endl;
    std::string demo = argv[2];
    std::string str = "python3 " + demo;
    system(str.c_str());
    std::cout << "End of python process" << std::endl;
    return 0;
}
