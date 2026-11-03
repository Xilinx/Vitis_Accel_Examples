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
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// XRT includes
#include "xrt/xrt_bo.h"
#include "xrt/xrt_device.h"
#include "xrt/xrt_hw_context.h"
#include "xrt/xrt_kernel.h"

typedef int data_t;

// Open an XRT device by index ("0") or BDF ("0000:c4:00.1").
static xrt::device open_device(const std::string& id) {
    if (id.find(":") != std::string::npos) return xrt::device(id);
    return xrt::device((unsigned int)stoi(id));
}

// Human-readable byte size, mirrors xcl::convert_size.
static std::string convert_size(size_t size) {
    static const char* suffix[] = {"B", "KB", "MB", "GB", "TB"};
    int i = 0;
    double dblByte = size;
    while (dblByte >= 1024 && i < 4) {
        dblByte /= 1024;
        i++;
    }
    std::ostringstream os;
    os << std::fixed << std::setprecision(0) << dblByte << " " << suffix[i];
    return os.str();
}

int main(int argc, char* argv[]) {
    // Command Line Parser
    sda::utils::CmdLineParser parser;

    // Switches
    //**************//"<Full Arg>",  "<Short Arg>", "<Description>", "<Default>"
    parser.addSwitch("--xclbin_file1", "-x1", "input binary file1 string", "");
    parser.addSwitch("--xclbin_file2", "-x2", "input binary file2 string", "");
    parser.addSwitch("--device0", "-d0", "first device id", "0");
    parser.addSwitch("--device1", "-d1", "second device id", "1");
    parser.parse(argc, argv);

    // Read settings
    auto binaryFile1 = parser.value("xclbin_file1");
    auto binaryFile2 = parser.value("xclbin_file2");
    std::string dev_id[2];
    dev_id[0] = parser.value("device0");
    dev_id[1] = parser.value("device1");

    if (argc < 5) {
        std::cout << "Options: <exe> <-x1> <first xclbin> <-x2> <second xclbin> "
                     "<optional> <-d0> <device id0> <-d1> <device id1>"
                  << std::endl;
        return EXIT_FAILURE;
    }

    int max_buffer = 16 * 1024 * 1024;
    size_t min_buffer = 4 * 1024;
    // xrt::device does not expose an emulation flag; detect via env, matching the
    // XCL_EMULATION_MODE set by the makefile emulation targets.
    bool is_emulation = (getenv("XCL_EMULATION_MODE") != nullptr);
    if (is_emulation) {
        max_buffer = 4 * 1024;
    }

    std::vector<data_t> in1(max_buffer);
    for (int i = 0; i < max_buffer; i++) {
        in1[i] = i;
    }

    // ---------------------------- Open both devices
    // -------------------------------------------
    // This design does P2P transfer between two devices. If the second device
    // cannot be opened, there is only one device present -- skip gracefully.
    xrt::device device[2];
    try {
        device[0] = open_device(dev_id[0]);
        device[1] = open_device(dev_id[1]);
    } catch (const std::exception& e) {
        std::cout << "WARNING: This design does P2P transfer between two devices. "
                     "Please run this "
                     "design on machine with two devices.\n";
        return 0;
    }

    int nodma_cnt = 0;
    bool dev0_nodma_chk = device[0].get_info<xrt::info::device::nodma>();
    bool dev1_nodma_chk = device[1].get_info<xrt::info::device::nodma>();
    if (dev0_nodma_chk) nodma_cnt++;
    if (dev1_nodma_chk) nodma_cnt++;

    if (nodma_cnt == 2) {
        std::cout
            << "WARNING: P2P transfer can only be done between xdma and nodma devices but not between 2 nodma devices. "
               "Please run this "
               "design on machine with both xdma and nodma devices.\n";
        return 0;
    }

    size_t vector_size_bytes = sizeof(data_t) * max_buffer;

    // ------------------------------- Program & kernels
    // -------------------------------------------
    auto uuid0 = device[0].load_xclbin(binaryFile1);
    xrt::hw_context ctx0(device[0], uuid0);
    std::cout << "device[0] program successful" << std::endl;
    auto krnl_dev0 = xrt::kernel(ctx0, "bandwidth");

    auto uuid1 = device[1].load_xclbin(binaryFile2);
    xrt::hw_context ctx1(device[1], uuid1);
    std::cout << "device[1] program successful" << std::endl;
    auto krnl_dev1 = xrt::kernel(ctx1, "bandwidth");

    // ---------------------------- Buffers
    // -------------------------------------------
    // bandwidth(buffer0, buffer1): arg0 = P2P buffer, arg1 = regular buffer.
    xrt::bo::flags p2p_flags = xrt::bo::flags::p2p;

    auto pbo1 = xrt::bo(device[0], vector_size_bytes, p2p_flags, krnl_dev0.group_id(0));
    auto rbo1 = xrt::bo(device[0], vector_size_bytes, krnl_dev0.group_id(1));

    auto pbo2 = xrt::bo(device[1], vector_size_bytes, p2p_flags, krnl_dev1.group_id(0));
    auto rbo2 = xrt::bo(device[1], vector_size_bytes, krnl_dev1.group_id(1));

    // Write input data to the regular buffers and migrate to device memory.
    std::cout << "Write input data to device global memory" << std::endl;
    rbo1.write(in1.data());
    rbo1.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    rbo2.write(in1.data());
    rbo2.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    //------------------------- P2P export / import
    //-----------------------------------------------------------
    // Export each device's P2P buffer and import it on the peer device so that
    // bo::copy performs the cross-device P2P DMA.
    auto pbo1_imported = xrt::bo(device[1], pbo1.export_buffer()); // pbo1 (dev0) imported on dev1
    auto pbo2_imported = xrt::bo(device[0], pbo2.export_buffer()); // pbo2 (dev1) imported on dev0

    size_t max_size = 128 * 1024 * 1024; // 128MB size
    std::cout << "Start P2P copy of various Buffer sizes \n";
    for (size_t bufsize = min_buffer; bufsize <= vector_size_bytes; bufsize *= 2) {
        std::string size_str = convert_size(bufsize);
        int iter = max_size / bufsize;
        if (is_emulation) {
            iter = 2; // Reducing iteration to run faster in emulation flow.
        }
        double dnsduration, dsduration, gbpersec;
        if (!dev0_nodma_chk) {
            //////////////////////// DMA Write by FPGA-1 //////////////////////////
            std::chrono::high_resolution_clock::time_point p2pReadStart = std::chrono::high_resolution_clock::now();
            for (int j = 0; j < iter; j++) {
                pbo2_imported.copy(rbo1, bufsize); // FPGA1 DMA Write
            }
            std::chrono::high_resolution_clock::time_point p2pReadEnd = std::chrono::high_resolution_clock::now();
            unsigned long p2pReadTime =
                std::chrono::duration_cast<std::chrono::microseconds>(p2pReadEnd - p2pReadStart).count();
            dnsduration = (double)p2pReadTime;
            dsduration = dnsduration / ((double)1000000);
            gbpersec = ((iter * bufsize) / dsduration) / ((double)1024 * 1024 * 1024);
            std::cout << "Buffer = " << size_str << " Iterations = " << iter
                      << " Total Data Transfer = " << convert_size(max_size)
                      << "\nDevice0 : DMA Write = " << std::setprecision(2) << std::fixed << gbpersec << "GB/s";

            //////////////////////// DMA Read by FPGA-1 //////////////////////////
            p2pReadStart = std::chrono::high_resolution_clock::now();
            for (int j = 0; j < iter; j++) {
                rbo1.copy(pbo2_imported, bufsize); // FPGA1 DMA Read
            }
            p2pReadEnd = std::chrono::high_resolution_clock::now();
            p2pReadTime = std::chrono::duration_cast<std::chrono::microseconds>(p2pReadEnd - p2pReadStart).count();
            dnsduration = (double)p2pReadTime;
            dsduration = dnsduration / ((double)1000000);
            gbpersec = ((iter * bufsize) / dsduration) / ((double)1024 * 1024 * 1024);
            std::cout << " DMA Read = " << std::setprecision(2) << std::fixed << gbpersec << "GB/s";

            //////////////////////// FPGA1 Read Write throughput /////////////////
            std::chrono::high_resolution_clock::time_point p2pReadWriteStart =
                std::chrono::high_resolution_clock::now();
            for (int j = 0; j < iter; j++) {
                pbo2_imported.copy(rbo1, bufsize); // FPGA1 DMA Write
                rbo1.copy(pbo2_imported, bufsize); // FPGA1 DMA Read
            }
            std::chrono::high_resolution_clock::time_point p2pReadWriteEnd = std::chrono::high_resolution_clock::now();
            unsigned long p2pReadWriteTime =
                std::chrono::duration_cast<std::chrono::microseconds>(p2pReadWriteEnd - p2pReadWriteStart).count();
            dnsduration = (double)p2pReadWriteTime;
            dsduration = dnsduration / ((double)1000000);
            gbpersec = ((2 * iter * bufsize) / dsduration) / ((double)1024 * 1024 * 1024);
            std::cout << " DMA Read Write = " << std::setprecision(2) << std::fixed << gbpersec << "GB/s\n";
        }
        if (!dev1_nodma_chk) {
            //////////////////////// DMA Write by FPGA-2 //////////////////////////
            std::chrono::high_resolution_clock::time_point p2pWriteStart = std::chrono::high_resolution_clock::now();
            for (int j = 0; j < iter; j++) {
                pbo1_imported.copy(rbo2, bufsize); // FPGA2 DMA Write
            }
            std::chrono::high_resolution_clock::time_point p2pWriteEnd = std::chrono::high_resolution_clock::now();
            unsigned long p2pWriteTime =
                std::chrono::duration_cast<std::chrono::microseconds>(p2pWriteEnd - p2pWriteStart).count();
            dnsduration = (double)p2pWriteTime;
            dsduration = dnsduration / ((double)1000000);
            gbpersec = ((iter * bufsize) / dsduration) / ((double)1024 * 1024 * 1024);
            std::cout << "Device1 : DMA Write = " << std::setprecision(2) << std::fixed << gbpersec << "GB/s";

            //////////////////////// DMA Read by FPGA-2 //////////////////////////
            p2pWriteStart = std::chrono::high_resolution_clock::now();
            for (int j = 0; j < iter; j++) {
                rbo2.copy(pbo1_imported, bufsize); // FPGA2 DMA Read
            }
            p2pWriteEnd = std::chrono::high_resolution_clock::now();
            p2pWriteTime = std::chrono::duration_cast<std::chrono::microseconds>(p2pWriteEnd - p2pWriteStart).count();
            dnsduration = (double)p2pWriteTime;
            dsduration = dnsduration / ((double)1000000);
            gbpersec = ((iter * bufsize) / dsduration) / ((double)1024 * 1024 * 1024);
            std::cout << " DMA Read = " << std::setprecision(2) << std::fixed << gbpersec << "GB/s";

            //////////////////////// FPGA2 Read Write throughput /////////////////
            std::chrono::high_resolution_clock::time_point p2pReadWriteStart =
                std::chrono::high_resolution_clock::now();
            for (int j = 0; j < iter; j++) {
                pbo1_imported.copy(rbo2, bufsize); // FPGA2 DMA Write
                rbo2.copy(pbo1_imported, bufsize); // FPGA2 DMA Read
            }
            std::chrono::high_resolution_clock::time_point p2pReadWriteEnd = std::chrono::high_resolution_clock::now();
            unsigned long p2pReadWriteTime =
                std::chrono::duration_cast<std::chrono::microseconds>(p2pReadWriteEnd - p2pReadWriteStart).count();
            dnsduration = (double)p2pReadWriteTime;
            dsduration = dnsduration / ((double)1000000);
            gbpersec = ((2 * iter * bufsize) / dsduration) / ((double)1024 * 1024 * 1024);
            std::cout << " DMA Read Write = " << std::setprecision(2) << std::fixed << gbpersec << "GB/s\n\n";
        }
    }

    std::cout << "Test passed!\n";
    return EXIT_SUCCESS;

} // end of main
