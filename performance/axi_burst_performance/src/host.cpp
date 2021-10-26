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
#include "xcl2.hpp"
#include <unistd.h>

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

        cl_int err;
        cl::CommandQueue q;
        cl::Context context;
        cl::Kernel krnl[6];

        if (xcl::is_emulation()) {
            buf_size_kb = 16;
        }

        if (buf_size_kb == 0) {
            buf_size_kb = buf_size_mb * 1024;
        }
        int64_t buf_size_bytes = buf_size_kb * 1024; // buffer size in bytes
        int64_t kernel_info[4];

        std::cout << "\nTest parameters\n";
        std::cout << " - xclbin file   : " << xclbinFile[p].c_str() << std::endl;
        std::cout << " - frequency     : " << frequency << " MHz" << std::endl;
        std::cout << " - buffer size   : " << xcl::convert_size(buf_size_bytes).c_str() << std::endl;
        std::cout << "\n";

        auto devices = xcl::get_xil_devices();
        auto device = devices[0];
        // read_binary_file() is a utility API which will load the binaryFile
        // and will return the pointer to file buffer.
        auto fileBuf = xcl::read_binary_file(xclbinFile[p]);
        cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
        bool valid_device = false;
        for (unsigned int i = 0; i < devices.size(); i++) {
            auto device = devices[i];
            // Creating Context and Command Queue for selected Device
            OCL_CHECK(err, context = cl::Context(device, nullptr, nullptr, nullptr, &err));
            OCL_CHECK(err, q = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err));

            std::cout << "Trying to program device[" << i << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
            cl::Program program(context, {device}, bins, nullptr, &err);
            if (err != CL_SUCCESS) {
                std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
            } else {
                std::cout << "Device[" << i << "]: program successful!\n";
                for (int i = 0; i < 6; i++) {
                    std::string krnl_name_full = "test_kernel_maxi_" + Data_Width[p] + "bit_" + std::to_string(i + 1);
                    OCL_CHECK(err, krnl[i] = cl::Kernel(program, krnl_name_full.c_str(), &err));
                }
                valid_device = true;
                break; // we break because we found a valid device
            }
        }
        if (!valid_device) {
            std::cerr << "Failed to program any device found, exit!\n";
            exit(EXIT_FAILURE);
        }

        // Create the buffers
        OCL_CHECK(err, cl::Buffer infoBuf(context, CL_MEM_WRITE_ONLY, sizeof(kernel_info), nullptr, &err));
        OCL_CHECK(err, cl::Buffer dataBuf(context, CL_MEM_READ_WRITE, buf_size_bytes, nullptr, &err));
        // Pin the buffers to kernel arguments
        for (int i = 0; i < 6; i++) {
            OCL_CHECK(err, err = krnl[i].setArg(2, infoBuf));
            OCL_CHECK(err, err = krnl[i].setArg(3, dataBuf));
        }
        // Make buffers resident in the device
        OCL_CHECK(err, err = q.enqueueMigrateMemObjects({infoBuf, dataBuf}, CL_MIGRATE_MEM_OBJECT_CONTENT_UNDEFINED,
                                                        nullptr, nullptr));
        q.finish();

        // Initialize data buffer
        char* dat = new char[buf_size_bytes];
        for (int i = 0; i < buf_size_bytes; i++) {
            dat[i] = 255;
        }
        OCL_CHECK(err, err = q.enqueueWriteBuffer(dataBuf, CL_TRUE, 0, buf_size_bytes, dat, nullptr, nullptr));

        std::string direction[] = {"WRITE", "READ"};

        for (int dir = 0; dir < 2; dir++) {
            std::cout << "\nKernel->AXI Burst " << direction[dir].c_str() << " performance" << std::endl;
            for (int id = 0; id < 6; id++) {
                // Run the test
                OCL_CHECK(err, err = krnl[id].setArg(0, buf_size_bytes));
                OCL_CHECK(err, err = krnl[id].setArg(1, dir));
                OCL_CHECK(err, err = q.enqueueTask(krnl[id]));
                q.finish();

                // Report results
                OCL_CHECK(err, err = q.enqueueReadBuffer(infoBuf, CL_TRUE, 0, sizeof(kernel_info), kernel_info, nullptr,
                                                         nullptr));
                int64_t duration_cy = kernel_info[0];
                double duration_ns = (double)(duration_cy * 1000) / frequency;
                double duration_sec = duration_ns / (1000 * 1000 * 1000);
                double throughput_bps = buf_size_bytes / duration_sec;
                double throughput_gbps = throughput_bps / (1024 * 1024 * 1024);
                errors = kernel_info[1];
                if (!xcl::is_emulation() or xcl::is_hw_emulation()) {
                    std::cout << "Data Width = " << Data_Width[p];
                    std::cout << " burst_length = " << kernel_info[2];
                    std::cout << " num_outstanding = " << kernel_info[3];
                    std::cout << " buffer_size = " << xcl::convert_size(buf_size_bytes).c_str();
                    std::cout << " | throughput = " << throughput_gbps << " GB/sec" << std::endl;
                }
                if (errors) {
                    std::cerr << "  ERROR: kernel return code !=0" << std::endl;
                }
            }
        }

        if (xcl::is_emulation() and !xcl::is_hw_emulation()) {
            std::cout << "\nNot reporting performance throughput for sw_emu as clock signal is not present for time "
                         "calculation."
                      << std::endl;
        }
    }

    std::cout << "\nTEST " << ((!errors) ? "PASSED" : "FAILED") << std::endl;
    return ((!errors) ? EXIT_SUCCESS : EXIT_FAILURE);
}
