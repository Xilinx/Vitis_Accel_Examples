/**
* Copyright (C) 2019-2021 Xilinx, Inc
* Copyright 2022-2026 Advanced Micro Devices, Inc.
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
#include <unistd.h>
#include <cstring>
#include <iostream>
#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"
#include "xrt/xrt_bo.h"
#include "xrt/xrt_hw_context.h"

int main(int argc, char** argv) {
    // Command Line Parser
    sda::utils::CmdLineParser parser;

    // Switches
    //**************//"<Full Arg>",  "<Short Arg>", "<Description>", "<Default>"
    parser.addSwitch("--xclbin_file_testKernel_256", "-x1", "testKernel_256 binary file string", "");
    parser.addSwitch("--xclbin_file_testKernel_512", "-x2", "testKernel_512 binary file string", "");
    parser.addSwitch("--frequency", "-f", "Operating frequency, in MHz", "300");
    parser.addSwitch("--buf_size_mb", "-m", "Test buffer size, in MB", "16");
    parser.addSwitch("--buf_size_kb", "-k", "Test buffer size, in KB", "0");
    parser.parse(argc, argv);

    std::string xclbinFile1 = parser.value("xclbin_file_testKernel_256");
    std::string xclbinFile2 = parser.value("xclbin_file_testKernel_512");
    std::string xclbinFile[] = {xclbinFile1, xclbinFile2};
    std::string Data_Width[] = {"256", "512"};
    float frequency = stof(parser.value("frequency"));
    int64_t buf_size_mb = stoi(parser.value("buf_size_mb"));
    int64_t buf_size_kb = stoi(parser.value("buf_size_kb"));

    if (argc < 3) {
        parser.printHelp();
        return EXIT_FAILURE;
    }

    int64_t errors = 0;

    for (int p = 0; p < 2; p++) {
        if (xclbinFile[p].empty()) {
            std::cerr << "ERROR: xclbin file must be specified with the -x" + std::to_string(p) + " option"
                      << std::endl;
            parser.printHelp();
            return EXIT_FAILURE;
        }
        if (access(xclbinFile[p].c_str(), R_OK) != 0) {
            std::cerr << "ERROR: " << xclbinFile[p].c_str() << " file not found" << std::endl;
            parser.printHelp();
            return EXIT_FAILURE;
        }

        xrt::kernel krnl[6];

        // Check if running in emulation mode
        const char* xcl_mode = std::getenv("XCL_EMULATION_MODE");
        bool is_emulation = (xcl_mode != nullptr);
        bool is_hw_emulation = (xcl_mode != nullptr && std::string(xcl_mode) == "hw_emu");

        if (is_emulation) {
            buf_size_kb = 16;
        }

        if (buf_size_kb == 0) {
            buf_size_kb = buf_size_mb * 1024;
        }
        int64_t buf_size_bytes = buf_size_kb * 1024; // buffer size in bytes
        int64_t kernel_info[4];

        // Helper function to convert size to human-readable format
        auto convert_size = [](size_t size) -> std::string {
            const char* units[] = {"B", "KB", "MB", "GB"};
            int unit_idx = 0;
            double dsize = static_cast<double>(size);
            while (dsize >= 1024.0 && unit_idx < 3) {
                dsize /= 1024.0;
                unit_idx++;
            }
            char buf[64];
            snprintf(buf, sizeof(buf), "%.2f %s", dsize, units[unit_idx]);
            return std::string(buf);
        };

        std::cout << "\nTest parameters\n";
        std::cout << " - xclbin file   : " << xclbinFile[p].c_str() << std::endl;
        std::cout << " - frequency     : " << frequency << " MHz" << std::endl;
        std::cout << " - buffer size   : " << convert_size(buf_size_bytes).c_str() << std::endl;
        std::cout << "\n";

        // Create XRT device and hw_context with xclbin
        xrt::device device(0);
        auto uuid = device.load_xclbin(xclbinFile[p]);
        xrt::hw_context hw_ctx(device, uuid);

        std::cout << "Device[0]: program successful!\n";

        // Create kernel objects
        for (int i = 0; i < 6; i++) {
            std::string krnl_name_full = "test_kernel_maxi_" + Data_Width[p] + "bit_" + std::to_string(i + 1);
            krnl[i] = xrt::kernel(hw_ctx, krnl_name_full);
        }

        // Create XRT buffer objects
        xrt::bo infoBuf(hw_ctx, sizeof(kernel_info), krnl[0].group_id(2));
        xrt::bo dataBuf(hw_ctx, buf_size_bytes, krnl[0].group_id(3));

        // Initialize data buffer using XRT BO map
        char* dat = dataBuf.map<char*>();
        for (int i = 0; i < buf_size_bytes; i++) {
            dat[i] = 255;
        }
        dataBuf.sync(XCL_BO_SYNC_BO_TO_DEVICE);

        std::string direction[] = {"WRITE", "READ"};

        for (int dir = 0; dir < 2; dir++) {
            std::cout << "\nKernel->AXI Burst " << direction[dir].c_str() << " performance" << std::endl;
            for (int id = 0; id < 6; id++) {
                // Run the test using XRT run API
                auto run = krnl[id](buf_size_bytes, dir, infoBuf, dataBuf);
                run.wait();

                // Report results - sync BO and read data
                infoBuf.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
                auto info_ptr = infoBuf.map<int64_t*>();
                std::memcpy(kernel_info, info_ptr, sizeof(kernel_info));

                int64_t duration_cy = kernel_info[0];
                double duration_ns = (double)(duration_cy * 1000) / frequency;
                double duration_sec = duration_ns / (1000 * 1000 * 1000);
                double throughput_bps = buf_size_bytes / duration_sec;
                double throughput_gbps = throughput_bps / (1024 * 1024 * 1024);
                errors = kernel_info[1];
                if (!is_emulation || is_hw_emulation) {
                    std::cout << "Data Width = " << Data_Width[p];
                    std::cout << " burst_length = " << kernel_info[2];
                    std::cout << " num_outstanding = " << kernel_info[3];
                    std::cout << " buffer_size = " << convert_size(buf_size_bytes).c_str();
                    std::cout << " | throughput = " << throughput_gbps << " GB/sec" << std::endl;
                }
                if (errors) {
                    std::cerr << "  ERROR: kernel return code !=0" << std::endl;
                }
            }
        }

        if (is_emulation && !is_hw_emulation) {
            std::cout << "\nNot reporting performance throughput for sw_emu as clock signal is not present for time "
                         "calculation."
                      << std::endl;
        }
    }

    std::cout << "\nTEST " << ((!errors) ? "PASSED" : "FAILED") << std::endl;
    return ((!errors) ? EXIT_SUCCESS : EXIT_FAILURE);
}
