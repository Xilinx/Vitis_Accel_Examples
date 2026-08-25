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
#include <cstring>
#include <iostream>
#include <chrono>

// XRT includes
#include "xrt/xrt_bo.h"
#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"
#include "xrt/xrt_hw_context.h"

// Helper to convert size to human-readable string
std::string convert_size(size_t size) {
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
}

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
    xrt::hw_context hw_ctx(device, uuid);

    auto krnl = xrt::kernel(hw_ctx, "bandwidth");
    auto krnl_read = xrt::kernel(hw_ctx, "read_bandwidth");
    auto krnl_write = xrt::kernel(hw_ctx, "write_bandwidth");

    double concurrent_max = 0;
    double read_max = 0;
    double write_max = 0;

    const char* xcl_mode = std::getenv("XCL_EMULATION_MODE");
    bool is_emulation = (xcl_mode != nullptr);

    for (size_t i = 4 * 1024; i <= 64 * 1024 * 1024; i *= 2) {
        size_t iter = (64 * 1024 * 1024) / i;
        size_t bufsize = i;

        if (is_emulation) {
            iter = 2;
            if (bufsize > 8 * 1024) break;
        }

        /* Input buffer */
        unsigned char* input_host = ((unsigned char*)malloc(bufsize));
        if (input_host == NULL) {
            std::cout << "Error: Failed to allocate host side copy of "
                      << "buffer of size " << bufsize << std::endl;
            return EXIT_FAILURE;
        }

        for (size_t i = 0; i < bufsize; i++) {
            input_host[i] = i % 256;
        }

        xrt::bo::flags flags = xrt::bo::flags::host_only;
        auto hostonly_bo_in = xrt::bo(hw_ctx, bufsize, flags, krnl.group_id(0));
        auto hostonly_bo_out = xrt::bo(hw_ctx, bufsize, flags, krnl.group_id(1));

        double dbytes = bufsize;
        std::string size_str = convert_size(bufsize);

        // Map the contents of the buffer object into host memory
        auto bo_in_map = hostonly_bo_in.map<char*>();
        auto bo_out_map = hostonly_bo_out.map<char*>();

        std::fill(bo_in_map, bo_in_map + bufsize, 0);
        std::fill(bo_out_map, bo_out_map + bufsize, 0);

        // Create the test data
        for (size_t i = 0; i < bufsize; ++i) {
            bo_in_map[i] = input_host[i];
        }

        auto start = std::chrono::high_resolution_clock::now();
        auto run = krnl(hostonly_bo_in, hostonly_bo_out, bufsize, iter);
        run.wait();
        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        double msduration = duration / iter;

        // Validate our results
        if (std::memcmp(bo_out_map, bo_in_map, bufsize))
            throw std::runtime_error("Value read back does not match reference");

        /* Profiling information */
        double dsduration = msduration / ((double)1000000);
        double bpersec = (dbytes / dsduration);
        double gbpersec = (2 * bpersec) / ((double)1024 * 1024 * 1024); // For Concurrent Read and Write

        std::cout << "Concurrent Read and Write Throughput = " << gbpersec << " (GB/sec) for buffer size " << size_str
                  << std::endl;

        if (gbpersec > concurrent_max) {
            concurrent_max = gbpersec;
        }

        start = std::chrono::high_resolution_clock::now();
        auto run_read = krnl_read(hostonly_bo_in, bufsize, iter);
        run_read.wait();
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        msduration = duration / iter;

        /* Profiling information */
        dsduration = msduration / ((double)1000000);
        bpersec = (dbytes / dsduration);
        gbpersec = (bpersec) / ((double)1024 * 1024 * 1024);

        std::cout << "Read Throughput = " << gbpersec << " (GB/sec) for buffer size " << size_str << std::endl;

        if (gbpersec > read_max) {
            read_max = gbpersec;
        }

        start = std::chrono::high_resolution_clock::now();
        auto run_write = krnl_write(hostonly_bo_out, bufsize, iter);
        run_write.wait();
        end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        msduration = duration / iter;

        /* Profiling information */
        dsduration = msduration / ((double)1000000);
        bpersec = (dbytes / dsduration);
        gbpersec = (bpersec) / ((double)1024 * 1024 * 1024);

        std::cout << "Write Throughput = " << gbpersec << " (GB/sec) for buffer size " << size_str << "\n\n";

        if (gbpersec > write_max) {
            write_max = gbpersec;
        }
    }

    std::cout << "Maximum bandwidth achieved :\n";
    std::cout << "Concurrent Read and Write Throughput = " << concurrent_max << " (GB/sec) \n";
    std::cout << "Read Throughput = " << read_max << " (GB/sec) \n";
    std::cout << "Write Throughput = " << write_max << " (GB/sec) \n\n";
    std::cout << "TEST PASSED\n";
    return EXIT_SUCCESS;
}
