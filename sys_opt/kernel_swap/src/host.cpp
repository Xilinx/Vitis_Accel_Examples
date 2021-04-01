/**
* Copyright (C) 2020 Xilinx, Inc
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
#include <vector>

#define LENGTH 1024

int main(int argc, char** argv) {
    // Command Line Parser
    sda::utils::CmdLineParser parser;

#ifdef dfx_device
    if (xcl::is_hw_emulation()) {
        std::cout << "INFO: This example is not supported for dfx platforms for hw_emu" << std::endl;
        return EXIT_SUCCESS;
    }
#endif

    // Switches
    //**************//"<Full Arg>",  "<Short Arg>", "<Description>", "<Default>"
    parser.addSwitch("--xclbin_file_krnl_mmult", "-x1", "krnl_mmult binary file string", "");
    parser.addSwitch("--xclbin_file_krnl_madd", "-x2", "krnl_madd binary file string", "");
    parser.parse(argc, argv);

    // Read settings
    auto binaryFile1 = parser.value("xclbin_file_krnl_mmult");
    auto binaryFile2 = parser.value("xclbin_file_krnl_madd");

    if (argc != 5) {
        parser.printHelp();
        return EXIT_FAILURE;
    }

    cl_int err;
    std::vector<int, aligned_allocator<int> > h_a(LENGTH);    // host memory for a vector
    std::vector<int, aligned_allocator<int> > h_b(LENGTH);    // host memory for b vector
    std::vector<int, aligned_allocator<int> > h_temp(LENGTH); // host memory for temp vector
    std::vector<int, aligned_allocator<int> > h_c(LENGTH);    // host memory for c vector

    // Fill our data sets with pattern
    int i = 0;
    for (i = 0; i < LENGTH; i++) {
        h_a[i] = i;
        h_b[i] = i;
        h_temp[i] = 0;
        h_c[i] = 0;
    }

    auto devices = xcl::get_xil_devices();
    auto device = devices[0];

    // Creating Context and Command Queue for selected Device
    OCL_CHECK(err, cl::Context context(device, nullptr, nullptr, nullptr, &err));
    OCL_CHECK(err, cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
    std::string device_name = device.getInfo<CL_DEVICE_NAME>();
    std::cout << "Found Device=" << device_name.c_str() << std::endl;

    int vector_length = LENGTH;
    bool match = true;

    // The temporary pointer(h_temp) is created mainly for the dynamic platforms,
    // since in the dynamic platforms we will not be able to load a second xclbin
    // unless all the cl buffers are released before calling cl::Program a second
    // time in the same process. The code block below is in braces because the cl
    // objects
    // are automatically released once the block ends
    {
        printf("INFO: loading vmul kernel\n");
        std::string vmulBinaryFile = binaryFile1.c_str();
        auto fileBuf = xcl::read_binary_file(vmulBinaryFile);
        cl::Program::Binaries vmul_bins{{fileBuf.data(), fileBuf.size()}};
        devices.resize(1);
        OCL_CHECK(err, cl::Program program(context, devices, vmul_bins, nullptr, &err));
        OCL_CHECK(err, cl::Kernel krnl_vmul(program, "krnl_vmul", &err));

        OCL_CHECK(err, cl::Buffer d_a(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, sizeof(int) * LENGTH, h_a.data(),
                                      &err));
        OCL_CHECK(err, cl::Buffer d_b(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, sizeof(int) * LENGTH, h_b.data(),
                                      &err));
        OCL_CHECK(err, cl::Buffer d_mul(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE, sizeof(int) * LENGTH,
                                        h_temp.data(), &err));

        OCL_CHECK(err, err = krnl_vmul.setArg(0, d_a));
        OCL_CHECK(err, err = krnl_vmul.setArg(1, d_b));
        OCL_CHECK(err, err = krnl_vmul.setArg(2, d_mul));
        OCL_CHECK(err, err = krnl_vmul.setArg(3, vector_length));

        OCL_CHECK(err, err = q.enqueueMigrateMemObjects({d_a, d_b}, 0 /* 0 means from host*/));

        // This function will execute the kernel on the FPGA
        OCL_CHECK(err, err = q.enqueueTask(krnl_vmul));

        OCL_CHECK(err, err = q.enqueueMigrateMemObjects({d_mul}, CL_MIGRATE_MEM_OBJECT_HOST));
        OCL_CHECK(err, err = q.finish());

        // Check Results
        for (int i = 0; i < LENGTH; i++) {
            if ((h_a[i] * h_b[i]) != h_temp[i]) {
                printf("ERROR in vmul - %d - a=%d, b=%d, c=%d\n", i, h_a[i], h_b[i], h_c[i]);
                match = false;
                break;
            }
        }
    }

    if (match) {
        printf("INFO: loading vadd_krnl\n");
        auto vaddBinaryFile = binaryFile2.c_str();
        auto fileBuf = xcl::read_binary_file(vaddBinaryFile);
        cl::Program::Binaries vadd_bins{{fileBuf.data(), fileBuf.size()}};
        cl::Program program(context, devices, vadd_bins);
        cl::Kernel krnl_vadd(program, "krnl_vadd");

        // Need to create the buffer and allocate the memory for the dynamic
        // platforms
        cl::Buffer d_temp(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, sizeof(int) * LENGTH, h_temp.data());
        cl::Buffer d_add(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, sizeof(int) * LENGTH, h_c.data());

        krnl_vadd.setArg(0, d_temp);
        krnl_vadd.setArg(1, d_temp);
        krnl_vadd.setArg(2, d_add);
        krnl_vadd.setArg(3, vector_length);

        q.enqueueMigrateMemObjects({d_temp}, 0 /* 0 means from host*/);

        // This function will execute the kernel on the FPGA
        q.enqueueTask(krnl_vadd);

        q.enqueueMigrateMemObjects({d_add}, CL_MIGRATE_MEM_OBJECT_HOST);
        q.finish();

        // Check Results
        for (int i = 0; i < LENGTH; i++) {
            if ((2 * (h_a[i] * h_b[i])) != h_c[i]) {
                printf("ERROR in vadd - %d - c=%d\n", i, h_c[i]);
                match = false;
                break;
            }
        }
    }

    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;
    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
