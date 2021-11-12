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

// OpenCL utility layer include
#include "cmdlineparser.h"
#include "xcl2.hpp"
#include <fcntl.h>
#include <fstream>
#include <iomanip>
#include <iosfwd>
#include <iostream>
#include <unistd.h>
#include <vector>

size_t max_buffer = 16 * 1024 * 1024;
size_t min_buffer = 4 * 1024;
size_t max_size = 128 * 1024 * 1024; // 128MB

int p2p_host_to_ssd(int& nvmeFd,
                    cl::Context context,
                    cl::CommandQueue q,
                    cl::Program program,
                    std::vector<int, aligned_allocator<int> > source_input_A) {
    int err;
    int ret = 0;
    size_t vector_size_bytes = sizeof(int) * max_buffer;

    cl::Kernel krnl;
    // Allocate Buffer in Global Memory
    cl_mem_ext_ptr_t outExt;
    outExt = {XCL_MEM_EXT_P2P_BUFFER, nullptr, 0};

    OCL_CHECK(err, cl::Buffer input_a(context, CL_MEM_READ_ONLY, vector_size_bytes, nullptr, &err));
    OCL_CHECK(err,
              cl::Buffer p2pBo(context, CL_MEM_WRITE_ONLY | CL_MEM_EXT_PTR_XILINX, vector_size_bytes, &outExt, &err));
    OCL_CHECK(err, krnl = cl::Kernel(program, "bandwidth", &err));

    int* inputPtr = (int*)q.enqueueMapBuffer(input_a, CL_TRUE, CL_MAP_WRITE | CL_MAP_READ, 0, vector_size_bytes,
                                             nullptr, nullptr, &err);

    for (uint32_t i = 0; i < max_buffer; i++) {
        inputPtr[i] = source_input_A[i];
    }
    q.finish();

    // Set the Kernel Arguments
    OCL_CHECK(err, err = krnl.setArg(0, input_a));
    OCL_CHECK(err, err = krnl.setArg(1, p2pBo));
    OCL_CHECK(err, err = krnl.setArg(2, max_buffer));

    // Copy input data to device global memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({input_a}, 0 /* 0 means from host*/));

    // Launch the Kernel
    OCL_CHECK(err, err = q.enqueueTask(krnl));

    std::cout << "\nMap P2P device buffers to host access pointers\n" << std::endl;
    void* p2pPtr = q.enqueueMapBuffer(p2pBo,                      // buffer
                                      CL_TRUE,                    // blocking call
                                      CL_MAP_WRITE | CL_MAP_READ, // Indicates we will be writing
                                      0,                          // buffer offset
                                      vector_size_bytes,          // size in bytes
                                      nullptr, nullptr,
                                      &err); // error code

    std::cout << "Start P2P Write of various buffer sizes from device buffers to SSD\n" << std::endl;
    for (size_t bufsize = min_buffer; bufsize <= vector_size_bytes; bufsize *= 2) {
        std::string size_str = xcl::convert_size(bufsize);

        int iter = max_size / bufsize;
        if (xcl::is_emulation()) {
            iter = 2; // Reducing iteration to run faster in emulation flow.
        }
        std::chrono::high_resolution_clock::time_point p2pStart = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iter; i++) {
            ret = pwrite(nvmeFd, (void*)p2pPtr, bufsize, 0);
            if (ret == -1) {
                std::cout << "P2P: write() failed, err: " << ret << ", line: " << __LINE__ << std::endl;
                return EXIT_FAILURE;
            }
        }
        std::chrono::high_resolution_clock::time_point p2pEnd = std::chrono::high_resolution_clock::now();
        cl_ulong p2pTime = std::chrono::duration_cast<std::chrono::microseconds>(p2pEnd - p2pStart).count();
        ;
        double dnsduration = (double)p2pTime;
        double dsduration = dnsduration / ((double)1000000);
        double gbpersec = (iter * bufsize / dsduration) / ((double)1024 * 1024 * 1024);
        std::cout << "Buffer = " << size_str << " Iterations = " << iter << " Throughput = " << std::setprecision(2)
                  << std::fixed << gbpersec << "GB/s\n";
    }
    return 0;
}

void p2p_ssd_to_host(int& nvmeFd,
                     cl::Context context,
                     cl::CommandQueue q,
                     cl::Program program,
                     std::vector<int, aligned_allocator<int> >* source_input_A) {
    int err;
    size_t vector_size_bytes = sizeof(int) * max_buffer;

    // Allocate Buffer in Global Memory
    cl_mem_ext_ptr_t inExt;
    inExt = {XCL_MEM_EXT_P2P_BUFFER, nullptr, 0};

    OCL_CHECK(err, cl::Buffer buffer_input(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, vector_size_bytes, &inExt,
                                           &err));

    std::cout << "\nMap P2P device buffers to host access pointers\n" << std::endl;
    void* p2pPtr1 = q.enqueueMapBuffer(buffer_input,      // buffer
                                       CL_TRUE,           // blocking call
                                       CL_MAP_READ,       // Indicates we will be writing
                                       0,                 // buffer offset
                                       vector_size_bytes, // size in bytes
                                       nullptr, nullptr,
                                       &err); // error code

    std::cout << "Start P2P Read of various buffer sizes from SSD to device buffers\n" << std::endl;
    for (size_t bufsize = min_buffer; bufsize <= vector_size_bytes; bufsize *= 2) {
        std::string size_str = xcl::convert_size(bufsize);

        int iter = max_size / bufsize;
        if (xcl::is_emulation()) {
            iter = 2; // Reducing iteration to run faster in emulation flow.
        }
        std::chrono::high_resolution_clock::time_point p2pStart = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iter; i++) {
            if (pread(nvmeFd, (void*)p2pPtr1, bufsize, 0) <= 0) {
                std::cerr << "ERR: pread failed: "
                          << " error: " << strerror(errno) << std::endl;
                exit(EXIT_FAILURE);
            }
        }
        std::chrono::high_resolution_clock::time_point p2pEnd = std::chrono::high_resolution_clock::now();
        cl_ulong p2pTime = std::chrono::duration_cast<std::chrono::microseconds>(p2pEnd - p2pStart).count();
        ;
        double dnsduration = (double)p2pTime;
        double dsduration = dnsduration / ((double)1000000);
        double gbpersec = (iter * bufsize / dsduration) / ((double)1024 * 1024 * 1024);
        std::cout << "Buffer = " << size_str << " Iterations = " << iter << " Throughput = " << std::setprecision(2)
                  << std::fixed << gbpersec << "GB/s\n";
    }
}

int main(int argc, char** argv) {
    // Command Line Parser
    sda::utils::CmdLineParser parser;

    // Switches
    //**************//"<Full Arg>",  "<Short Arg>", "<Description>", "<Default>"
    parser.addSwitch("--xclbin_file", "-x", "input binary file string", "");
    parser.addSwitch("--file_path", "-p", "file path string", "");
    parser.addSwitch("--input_file", "-f", "input file string", "");
    parser.addSwitch("--device", "-d", "device id", "0");
    parser.parse(argc, argv);

    // Read settings
    auto binaryFile = parser.value("xclbin_file");
    std::string filepath = parser.value("file_path");
    std::string dev_id = parser.value("device");
    std::string filename;

    if (argc < 5) {
        parser.printHelp();
        return EXIT_FAILURE;
    }

    if (filepath.empty()) {
        std::cout << "\nWARNING: As file path is not provided using -p option, going with -f option which is local "
                     "file testing. Please use -p option, if looking for actual p2p operation on NVMe drive.\n";
        filename = parser.value("input_file");
    } else {
        std::cout << "\nWARNING: Ignoring -f option when -p options is set. -p has high precedence over -f.\n";
        filename = filepath;
    }

    int nvmeFd = -1;
    if (xcl::is_emulation()) {
        max_buffer = 16 * 1024;
    }

    cl_int err;
    cl::Context context;
    cl::CommandQueue q;
    std::vector<int, aligned_allocator<int> > source_input_A(max_buffer);

    // OPENCL HOST CODE AREA START
    // get_xil_devices() is a utility API which will find the xilinx
    // platforms and will return list of devices connected to Xilinx platform
    auto devices = xcl::get_xil_devices();
    // read_binary_file() is a utility API which will load the binaryFile
    // and will return the pointer to file buffer.
    auto fileBuf = xcl::read_binary_file(binaryFile);
    cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    cl::Program program;

    auto pos = dev_id.find(":");
    cl::Device device;
    if (pos == std::string::npos) {
        uint32_t device_index = stoi(dev_id);
        if (device_index >= devices.size()) {
            std::cout << "The device_index provided using -d flag is outside the range of "
                         "available devices\n";
            return EXIT_FAILURE;
        }
        device = devices[device_index];
    } else {
        if (xcl::is_emulation()) {
            std::cout << "Device bdf is not supported for the emulation flow\n";
            return EXIT_FAILURE;
        }
        device = xcl::find_device_bdf(devices, dev_id);
    }

    if (xcl::is_hw_emulation()) {
        auto device_name = device.getInfo<CL_DEVICE_NAME>();
        if (device_name.find("2018") != std::string::npos) {
            std::cout << "[INFO]: The example is not supported for " << device_name
                      << " this platform for hw_emu. Please try other flows." << '\n';
            return EXIT_SUCCESS;
        }
    }

    // Creating Context and Command Queue for selected Device
    OCL_CHECK(err, context = cl::Context(device, nullptr, nullptr, nullptr, &err));
    OCL_CHECK(err, q = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
    std::cout << "Trying to program device[" << dev_id << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
    program = cl::Program(context, {device}, bins, nullptr, &err);
    if (err != CL_SUCCESS) {
        std::cout << "Failed to program device[" << dev_id << "] with xclbin file!\n";
        exit(EXIT_FAILURE);
    } else
        std::cout << "Device[" << dev_id << "]: program successful!\n";

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
    int ret = 0;
    ret = p2p_host_to_ssd(nvmeFd, context, q, program, source_input_A);
    (void)close(nvmeFd);
    if (ret != 0) return EXIT_FAILURE;

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

    p2p_ssd_to_host(nvmeFd, context, q, program, &source_input_A);

    (void)close(nvmeFd);

    std::cout << "TEST PASSED" << std::endl;
    return EXIT_SUCCESS;
}
