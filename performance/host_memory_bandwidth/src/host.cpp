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

#include <iostream>
#include <cstring>
#include <chrono>
#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"
#include "xrt/xrt_bo.h"
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
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <xclbin>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];

    // XRT Native API initialization
    xrt::device device(2);
    auto uuid = device.load_xclbin(binaryFile);
    xrt::hw_context hw_ctx(device, uuid);

    std::cout << "Device[0]: program successful!\n";

    xrt::kernel krnl(hw_ctx, "bandwidth");
    xrt::kernel krnl_read(hw_ctx, "read_bandwidth");
    xrt::kernel krnl_write(hw_ctx, "write_bandwidth");

    double concurrent_max = 0;
    double read_max = 0;
    double write_max = 0;

    const char* xcl_mode = std::getenv("XCL_EMULATION_MODE");
    bool is_emulation = (xcl_mode != nullptr);

    for (size_t i = 4 * 1024; i <= 256 * 1024 * 1024; i *= 2) {
        size_t iter = 1024;
        size_t bufsize = i;

        if (is_emulation) {
            iter = 2;
            if (bufsize > 8 * 1024) break;
        }

        /* Input buffer */
        unsigned char* input_host = ((unsigned char*)malloc(bufsize));
        if (input_host == nullptr) {
            std::cout << "Error: Failed to allocate host side copy of OpenCL source "
                      << "buffer of size " << bufsize << std::endl;
            return EXIT_FAILURE;
        }

        for (size_t i = 0; i < bufsize; i++) {
            input_host[i] = i % 256;
        }

        // Create XRT buffer objects with host memory flags
        xrt::bo buffer0(hw_ctx, bufsize, xrt::bo::flags::host_only, krnl.group_id(0));
        xrt::bo buffer1(hw_ctx, bufsize, xrt::bo::flags::host_only, krnl.group_id(1));

        double dbytes = bufsize;
        std::string size_str = convert_size(bufsize);

        /* Write input buffer */
        auto map_input_buffer0 = buffer0.map<unsigned char*>();
        for (size_t i = 0; i < bufsize; i++) {
            map_input_buffer0[i] = input_host[i];
        }
        buffer0.sync(XCL_BO_SYNC_BO_TO_DEVICE);

        /* Execute Kernel */
        auto start = std::chrono::high_resolution_clock::now();
        auto run = krnl(buffer0, buffer1, bufsize, iter);
        run.wait();
        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        double msduration = duration / iter;

        /* Copy results back from buffer */
        buffer1.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
        auto map_output_buffer0 = buffer1.map<unsigned char*>();

        /* Check the results of output0 */
        for (size_t i = 0; i < bufsize; i++) {
            if (map_output_buffer0[i] != input_host[i]) {
                std::cout << "ERROR : kernel failed to copy entry " << i << " input " << input_host[i] << " output "
                          << map_output_buffer0[i] << std::endl;
                return EXIT_FAILURE;
            }
        }

        /* Profiling information */
        double dsduration = msduration / ((double)1000000);

        double bpersec = (dbytes / dsduration);
        double gbpersec = (2 * bpersec) / ((double)1024 * 1024 * 1024); // For Concurrent Read and Write

        std::cout << "Concurrent Read and Write Throughput = " << gbpersec << " (GB/sec) for buffer size " << size_str
                  << std::endl;

        if (gbpersec > concurrent_max) {
            concurrent_max = gbpersec;
        }

        /* Execute read kernel */
        auto read_start = std::chrono::high_resolution_clock::now();
        auto read_run = krnl_read(buffer0, bufsize, iter);
        read_run.wait();
        auto read_end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(read_end - read_start).count();
        msduration = duration / iter;

        /* Profiling information */
        dsduration = msduration / ((double)1000000);

        bpersec = (dbytes / dsduration);
        gbpersec = bpersec / ((double)1024 * 1024 * 1024);

        std::cout << "Read Throughput = " << gbpersec << " (GB/sec) for buffer size " << size_str << std::endl;

        if (gbpersec > read_max) {
            read_max = gbpersec;
        }

        /* Execute write kernel */
        auto write_start = std::chrono::high_resolution_clock::now();
        auto write_run = krnl_write(buffer1, bufsize, iter);
        write_run.wait();
        auto write_end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(write_end - write_start).count();
        msduration = duration / iter;

        /* Profiling information */
        dsduration = msduration / ((double)1000000);

        bpersec = (dbytes / dsduration);
        gbpersec = bpersec / ((double)1024 * 1024 * 1024);

        std::cout << "Write Throughput = " << gbpersec << " (GB/sec) for buffer size " << size_str << "\n\n";

        if (gbpersec > write_max) {
            write_max = gbpersec;
        }

        free(input_host);
    }

    std::cout << "Maximum bandwidth achieved :\n";
    std::cout << "Concurrent Read and Write Throughput = " << concurrent_max << " (GB/sec) \n";
    std::cout << "Read Throughput = " << read_max << " (GB/sec) \n";
    std::cout << "Write Throughput = " << write_max << " (GB/sec) \n\n";
    std::cout << "TEST PASSED\n";
    return EXIT_SUCCESS;
}
