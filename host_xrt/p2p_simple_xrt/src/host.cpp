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

#include <fcntl.h>
#include <fstream>
#include <iomanip>
#include <iosfwd>
#include <unistd.h>

#define DATA_SIZE 4096
#define INCR_VALUE 10

void p2p_host_to_ssd(int& nvmeFd, xrtDeviceHandle device, xrt::kernel& krnl) {
    // int err;
    int ret = 0, inc = INCR_VALUE;
    int size = DATA_SIZE;
    size_t vector_size_bytes = sizeof(int) * DATA_SIZE;

    xrt::bo::flags flags = xrt::bo::flags::p2p;
    auto bo0 = xrt::bo(device, vector_size_bytes, krnl.group_id(0));
    auto p2p_bo1 = xrt::bo(device, vector_size_bytes, flags, krnl.group_id(1));

    // Map the contents of the buffer object into host memory
    auto bo0_map = bo0.map<int*>();
    auto p2p_bo1_map = p2p_bo1.map<int*>();
    std::fill(bo0_map, bo0_map + size, 0);
    std::fill(p2p_bo1_map, p2p_bo1_map + size, 0);

    // Create the test data
    for (int i = 0; i < size; ++i) {
        bo0_map[i] = 15;
    }

    // Synchronize buffer content with device side
    std::cout << "synchronize input buffer data to device global memory\n";
    bo0.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    std::cout << "Execution of the kernel\n";
    auto run = krnl(bo0, p2p_bo1, inc, size);
    run.wait();

    std::cout << "Now start P2P Write from device buffers to SSD\n" << std::endl;
    ret = pwrite(nvmeFd, (void*)p2p_bo1_map, vector_size_bytes, 0);
    if (ret == -1) std::cout << "P2P: write() failed, err: " << ret << ", line: " << __LINE__ << std::endl;
}

void p2p_ssd_to_host(int& nvmeFd, xrtDeviceHandle device, xrt::kernel& krnl) {
    int inc = INCR_VALUE;
    int size = DATA_SIZE;
    size_t vector_size_bytes = sizeof(int) * DATA_SIZE;

    xrt::bo::flags flags = xrt::bo::flags::p2p;
    std::cout << "Allocate Buffer in Global Memory\n";
    auto p2p_bo0 = xrt::bo(device, vector_size_bytes, flags, krnl.group_id(0));
    auto bo_out = xrt::bo(device, vector_size_bytes, krnl.group_id(1));

    // Map the contents of the buffer object into host memory
    auto p2p_bo0_map = p2p_bo0.map<int*>();
    auto bo_out_map = bo_out.map<int*>();
    std::fill(p2p_bo0_map, p2p_bo0_map + size, 0);
    std::fill(bo_out_map, bo_out_map + size, 0);

    // Create the test data
    int bufReference[size];
    for (int i = 0; i < size; ++i) {
        p2p_bo0_map[i] = 0;
        bo_out_map[i] = 0;
        bufReference[i] = 15 + 2 * inc;
    }
    std::cout << "Now start P2P Read from SSD to device buffers\n" << std::endl;
    if (pread(nvmeFd, (void*)p2p_bo0_map, vector_size_bytes, 0) <= 0) {
        std::cerr << "ERR: pread failed: "
                  << " error: " << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Execution of the kernel\n";
    auto run1 = krnl(p2p_bo0, bo_out, inc, size);
    run1.wait();

    // Get the output;
    std::cout << "Get the output data from the device" << std::endl;
    bo_out.sync(XCL_BO_SYNC_BO_FROM_DEVICE);

    // Validate our results
    if (std::memcmp(bo_out_map, bufReference, size))
        throw std::runtime_error("Value read back does not match reference");
}

int main(int argc, char** argv) {
    // Command Line Parser
    sda::utils::CmdLineParser parser;

    // Switches
    //**************//"<Full Arg>",  "<Short Arg>", "<Description>", "<Default>"
    parser.addSwitch("--xclbin_file", "-x", "input binary file string", "");
    parser.addSwitch("--device_id", "-d", "device index", "0");
    parser.addSwitch("--file_path", "-p", "file path string", "");
    parser.addSwitch("--input_file", "-f", "input file string", "");
    parser.parse(argc, argv);

    // Read settings
    std::string binaryFile = parser.value("xclbin_file");
    int device_index = stoi(parser.value("device_id"));
    std::string filepath = parser.value("file_path");
    std::string filename;

    if (argc < 5) {
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
    std::cout << "Load the xclbin " << binaryFile << std::endl;
    auto uuid = device.load_xclbin(binaryFile);

    auto krnl = xrt::kernel(device, uuid, "adder");

    if (filepath.empty()) {
        std::cout << "\nWARNING: As file path is not provided using -p option, going with -f option which is local "
                     "file testing. Please use -p option, if looking for actual p2p operation on NVMe drive.\n";
        filename = parser.value("input_file");
    } else {
        std::cout << "\nWARNING: Ignoring -f option when -p options is set. -p has high precedence over -f.\n";
        filename = filepath;
    }

    int nvmeFd = -1;
    // P2P transfer from host to SSD
    std::cout << "############################################################\n";
    std::cout << "                  Writing data to SSD                       \n";
    std::cout << "############################################################\n";
    // Get access to the NVMe SSD.
    nvmeFd = open(filename.c_str(), O_RDWR | O_DIRECT);
    if (nvmeFd < 0) {
        std::cerr << "ERROR: open " << filename << "failed: " << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "INFO: Successfully opened NVME SSD " << filename << std::endl;
    p2p_host_to_ssd(nvmeFd, device, krnl);
    (void)close(nvmeFd);

    // P2P transfer from SSD to host
    std::cout << "############################################################\n";
    std::cout << "                  Reading data from SSD                       \n";
    std::cout << "############################################################\n";

    nvmeFd = open(filename.c_str(), O_RDWR | O_DIRECT);
    if (nvmeFd < 0) {
        std::cerr << "ERROR: open " << filename << "failed: " << std::endl;
        return EXIT_FAILURE;
    }
    std::cout << "INFO: Successfully opened NVME SSD " << filename << std::endl;

    p2p_ssd_to_host(nvmeFd, device, krnl);

    (void)close(nvmeFd);

    std::cout << "TEST PASSED\n";
    return 0;
}
