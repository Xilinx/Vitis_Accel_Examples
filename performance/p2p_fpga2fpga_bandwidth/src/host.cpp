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
#include <iomanip>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>

typedef int data_t;
decltype(&xclGetMemObjectFd) xcl::P2P::getMemObjectFd = nullptr;
decltype(&xclGetMemObjectFromFd) xcl::P2P::getMemObjectFromFd = nullptr;

cl_program xcl_import_binary_file(cl_device_id device_id, cl_context context, const char* xclbin_file_name);

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
    if (xcl::is_emulation()) {
        max_buffer = 4 * 1024;
    }

    std::vector<data_t, aligned_allocator<data_t> > in1(max_buffer);
    std::vector<data_t, aligned_allocator<data_t> > out1(max_buffer);
    for (int i = 0; i < max_buffer; i++) {
        in1[i] = i;
        out1[i] = 0;
    }

    cl_platform_id platform_id;
    cl_platform_id platforms[16] = {0};
    cl_uint platform_count;
    char platformName[256];
    cl_int error;

    clGetPlatformIDs(16, platforms, &platform_count);

    for (cl_uint i = 0; i < platform_count; i++) {
        error = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 256, platformName, 0);
        if (error != CL_SUCCESS) {
            exit(EXIT_FAILURE);
        }

        if (strcmp(platformName, "Xilinx") == 0) {
            platform_id = platforms[i];
        }
    }
    cl_uint device_count;
    clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ACCELERATOR, 0, nullptr, &device_count);
    std::cout << "Device count - " << device_count << std::endl;

    if (device_count < 2) {
        std::cout << "WARNING: This design does P2P transfer between two devices. "
                     "Please run this "
                     "design on machine with two devices.\n";
        return 0;
    }

    cl_device_id* device_id = (cl_device_id*)malloc(sizeof(cl_device_id) * device_count);
    clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_ACCELERATOR, device_count, device_id, nullptr);

    cl_device_id device[2];

    for (int i = 0; i < 2; i++) {
        auto pos = dev_id[i].find(":");
        if (pos == std::string::npos) {
            uint32_t device_index = stoi(dev_id[i]);
            if (device_index >= device_count) {
                std::cout << "The device_index provided using -d flag is outside the range of "
                             "available devices\n";
                return EXIT_FAILURE;
            }
            device[i] = device_id[device_index];
        } else
            device[i] = xcl::find_device_bdf_c(device_id, dev_id[i], device_count);
    }

    cl_bool is_nodma;
    uint8_t nodma_cnt = 0;
    clGetDeviceInfo(device[0], CL_DEVICE_NODMA, sizeof(is_nodma), &is_nodma, nullptr);
    if (is_nodma) nodma_cnt++;
    clGetDeviceInfo(device[1], CL_DEVICE_NODMA, sizeof(is_nodma), &is_nodma, nullptr);
    if (is_nodma) nodma_cnt++;

    if (nodma_cnt == 2) {
        std::cout
            << "WARNING: P2P transfer can only be done between xdma and nodma devices but not between 2 nodma devices. "
               "Please run this "
               "design on machine with both xdma and nodma devices.\n";
        return 0;
    }

    cl_context context[2];
    cl_command_queue queue[2];
    cl_kernel krnl_dev0, krnl_dev1;
    cl_program program[2];
    int err;

    OCL_CHECK(err, context[0] = clCreateContext(0, 1, &device[0], nullptr, nullptr, &err));
    if (err != CL_SUCCESS) std::cout << "clCreateContext call: Failed to create a compute context" << err << std::endl;
    OCL_CHECK(err, queue[0] = clCreateCommandQueue(context[0], device[0], CL_QUEUE_PROFILING_ENABLE, &err));
    if (err != CL_SUCCESS) std::cout << "clCreateCommandQueue call: Failed to create commandqueue" << err << std::endl;

    OCL_CHECK(err, context[1] = clCreateContext(0, 1, &device[1], nullptr, nullptr, &err));
    if (err != CL_SUCCESS) std::cout << "clCreateContext call: Failed to create a compute context" << err << std::endl;
    OCL_CHECK(err, queue[1] = clCreateCommandQueue(context[1], device[1], CL_QUEUE_PROFILING_ENABLE, &err));
    if (err != CL_SUCCESS) std::cout << "clCreateCommandQueue call: Failed to create commandqueue" << err << std::endl;

    size_t vector_size_bytes = sizeof(data_t) * max_buffer;
    //------------------------------- Program
    //-------------------------------------------
    program[0] = xcl_import_binary_file(device[0], context[0], binaryFile1.c_str());
    std::cout << "device[0] program successful" << std::endl;
    OCL_CHECK(err, krnl_dev0 = clCreateKernel(program[0], "bandwidth", &err));
    program[1] = xcl_import_binary_file(device[1], context[1], binaryFile2.c_str());
    std::cout << "device[1] program successful" << std::endl;
    OCL_CHECK(err, krnl_dev1 = clCreateKernel(program[1], "bandwidth", &err));

    xcl::P2P::init(platform_id);

    // ---------------------------- Buffer-1
    // -------------------------------------------
    cl_mem pbo1;
    cl_mem_ext_ptr_t pbo1_ext = {XCL_MEM_EXT_P2P_BUFFER, nullptr, 0};
    OCL_CHECK(err, pbo1 = clCreateBuffer(context[0], CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, vector_size_bytes,
                                         &pbo1_ext, &err));

    cl_mem rbo1;
    OCL_CHECK(err, rbo1 = clCreateBuffer(context[0], CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE, vector_size_bytes,
                                         in1.data(), &err));

    // ---------------------------- Buffer-2
    // -------------------------------------------
    cl_mem pbo2;
    cl_mem_ext_ptr_t pbo2_ext = {XCL_MEM_EXT_P2P_BUFFER, nullptr, 0};
    OCL_CHECK(err, pbo2 = clCreateBuffer(context[1], CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, vector_size_bytes,
                                         &pbo2_ext, &err));

    cl_mem rbo2;
    OCL_CHECK(err, rbo2 = clCreateBuffer(context[1], CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE, vector_size_bytes,
                                         in1.data(), &err));

    // ----------------------------Set Args
    // -------------------------------------------
    OCL_CHECK(err, err = clSetKernelArg(krnl_dev0, 0, sizeof(cl_mem), &pbo1));
    OCL_CHECK(err, err = clSetKernelArg(krnl_dev0, 1, sizeof(cl_mem), &rbo1));

    OCL_CHECK(err, err = clSetKernelArg(krnl_dev1, 0, sizeof(cl_mem), &pbo2));
    OCL_CHECK(err, err = clSetKernelArg(krnl_dev1, 1, sizeof(cl_mem), &rbo2));

    // -----------------------------------------------------------------------
    std::cout << "Write input data to device global memory" << std::endl;
    OCL_CHECK(err, err = clEnqueueMigrateMemObjects(queue[0], 1, &rbo1, CL_MIGRATE_MEM_OBJECT_CONTENT_UNDEFINED, 0,
                                                    nullptr, nullptr)); // Send data to the regular buffer of Device1
    OCL_CHECK(err, err = clEnqueueMigrateMemObjects(queue[1], 1, &rbo2, CL_MIGRATE_MEM_OBJECT_CONTENT_UNDEFINED, 0,
                                                    nullptr, nullptr)); // Send data to the regular buffer of Device2

    //------------------------- P2P
    //-----------------------------------------------------------
    int fd = -1;
    OCL_CHECK(err, err = xcl::P2P::getMemObjectFd(pbo1, &fd)); // Export p2p buffer to file descriptor (fd)

    cl_mem pbo1_imported;
    OCL_CHECK(err, err = xcl::P2P::getMemObjectFromFd(context[1], device[1], 0, fd, &pbo1_imported)); // Import

    int fd1 = -1;
    OCL_CHECK(err, err = xcl::P2P::getMemObjectFd(pbo2, &fd1)); // Export p2p buffer to file descriptor (fd)

    cl_mem pbo2_imported;
    OCL_CHECK(err, err = xcl::P2P::getMemObjectFromFd(context[0], device[0], 0, fd1, &pbo2_imported)); // Import

    size_t max_size = 128 * 1024 * 1024; // 128MB size
    std::cout << "Start P2P copy of various Buffer sizes \n";
    for (size_t bufsize = min_buffer; bufsize <= vector_size_bytes; bufsize *= 2) {
        std::string size_str = xcl::convert_size(bufsize);
        int iter = max_size / bufsize;
        if (xcl::is_emulation()) {
            iter = 2; // Reducing iteration to run faster in emulation flow.
        }

        //////////////////////// DMA Write by FPGA-1 //////////////////////////
        std::chrono::high_resolution_clock::time_point p2pReadStart = std::chrono::high_resolution_clock::now();
        for (int j = 0; j < iter; j++) {
            OCL_CHECK(err, err = clEnqueueCopyBuffer(queue[0], rbo1, pbo2_imported, 0, 0, bufsize, 0, nullptr,
                                                     nullptr)); // FPGA1 DMA Write
        }
        clFinish(queue[0]);
        std::chrono::high_resolution_clock::time_point p2pReadEnd = std::chrono::high_resolution_clock::now();
        cl_ulong p2pReadTime = std::chrono::duration_cast<std::chrono::microseconds>(p2pReadEnd - p2pReadStart).count();
        ;
        double dnsduration = (double)p2pReadTime;
        double dsduration = dnsduration / ((double)1000000);
        double gbpersec = ((iter * bufsize) / dsduration) / ((double)1024 * 1024 * 1024);
        std::cout << "Buffer = " << size_str << " Iterations = " << iter
                  << " Total Data Transfer = " << xcl::convert_size(max_size)
                  << "\nDevice0 : DMA Write = " << std::setprecision(2) << std::fixed << gbpersec << "GB/s";

        //////////////////////// DMA Read by FPGA-1 //////////////////////////
        p2pReadStart = std::chrono::high_resolution_clock::now();
        for (int j = 0; j < iter; j++) {
            OCL_CHECK(err, err = clEnqueueCopyBuffer(queue[0], pbo2_imported, rbo1, 0, 0, bufsize, 0, nullptr,
                                                     nullptr)); // FPGA1 DMA Read
        }
        clFinish(queue[0]);
        p2pReadEnd = std::chrono::high_resolution_clock::now();
        p2pReadTime = std::chrono::duration_cast<std::chrono::microseconds>(p2pReadEnd - p2pReadStart).count();
        ;
        dnsduration = (double)p2pReadTime;
        dsduration = dnsduration / ((double)1000000);
        gbpersec = ((iter * bufsize) / dsduration) / ((double)1024 * 1024 * 1024);
        std::cout << " DMA Read = " << std::setprecision(2) << std::fixed << gbpersec << "GB/s";

        //////////////////////// FPGA1 Read Write throughput /////////////////
        std::chrono::high_resolution_clock::time_point p2pReadWriteStart = std::chrono::high_resolution_clock::now();
        for (int j = 0; j < iter; j++) {
            OCL_CHECK(err, err = clEnqueueCopyBuffer(queue[0], rbo1, pbo2_imported, 0, 0, bufsize, 0, nullptr,
                                                     nullptr)); // FPGA1 DMA Write
            OCL_CHECK(err, err = clEnqueueCopyBuffer(queue[0], pbo2_imported, rbo1, 0, 0, bufsize, 0, nullptr,
                                                     nullptr)); // FPGA1 DMA Read
        }
        clFinish(queue[0]);

        std::chrono::high_resolution_clock::time_point p2pReadWriteEnd = std::chrono::high_resolution_clock::now();
        cl_ulong p2pReadWriteTime =
            std::chrono::duration_cast<std::chrono::microseconds>(p2pReadWriteEnd - p2pReadWriteStart).count();
        ;
        dnsduration = (double)p2pReadWriteTime;
        dsduration = dnsduration / ((double)1000000);
        gbpersec = ((2 * iter * bufsize) / dsduration) / ((double)1024 * 1024 * 1024);
        std::cout << " DMA Read Write = " << std::setprecision(2) << std::fixed << gbpersec << "GB/s\n";
        //////////////////////// DMA Write by FPGA-2 //////////////////////////
        std::chrono::high_resolution_clock::time_point p2pWriteStart = std::chrono::high_resolution_clock::now();
        for (int j = 0; j < iter; j++) {
            OCL_CHECK(err, err = clEnqueueCopyBuffer(queue[1], rbo2, pbo1_imported, 0, 0, bufsize, 0, nullptr,
                                                     nullptr)); // FPGA2 DMA Write
        }
        clFinish(queue[1]);
        std::chrono::high_resolution_clock::time_point p2pWriteEnd = std::chrono::high_resolution_clock::now();
        cl_ulong p2pWriteTime =
            std::chrono::duration_cast<std::chrono::microseconds>(p2pWriteEnd - p2pWriteStart).count();
        dnsduration = (double)p2pWriteTime;
        dsduration = dnsduration / ((double)1000000);
        gbpersec = ((iter * bufsize) / dsduration) / ((double)1024 * 1024 * 1024);
        std::cout << "Device1 : DMA Write = " << std::setprecision(2) << std::fixed << gbpersec << "GB/s";

        //////////////////////// DMA Read by FPGA-2 //////////////////////////
        p2pWriteStart = std::chrono::high_resolution_clock::now();
        for (int j = 0; j < iter; j++) {
            OCL_CHECK(err, err = clEnqueueCopyBuffer(queue[1], pbo1_imported, rbo2, 0, 0, bufsize, 0, nullptr,
                                                     nullptr)); // FPGA2 DMA Read
        }
        clFinish(queue[1]);
        p2pWriteEnd = std::chrono::high_resolution_clock::now();
        p2pWriteTime = std::chrono::duration_cast<std::chrono::microseconds>(p2pWriteEnd - p2pWriteStart).count();
        dnsduration = (double)p2pWriteTime;
        dsduration = dnsduration / ((double)1000000);
        gbpersec = ((iter * bufsize) / dsduration) / ((double)1024 * 1024 * 1024);
        std::cout << " DMA Read = " << std::setprecision(2) << std::fixed << gbpersec << "GB/s";

        //////////////////////// FPGA2 Read Write throughput /////////////////
        p2pReadWriteStart = std::chrono::high_resolution_clock::now();
        for (int j = 0; j < iter; j++) {
            OCL_CHECK(err, err = clEnqueueCopyBuffer(queue[1], rbo2, pbo1_imported, 0, 0, bufsize, 0, nullptr,
                                                     nullptr)); // FPGA2 DMA Write
            OCL_CHECK(err, err = clEnqueueCopyBuffer(queue[1], pbo1_imported, rbo2, 0, 0, bufsize, 0, nullptr,
                                                     nullptr)); // FPGA2 DMA Read
        }
        clFinish(queue[1]);
        p2pReadWriteEnd = std::chrono::high_resolution_clock::now();
        p2pReadWriteTime =
            std::chrono::duration_cast<std::chrono::microseconds>(p2pReadWriteEnd - p2pReadWriteStart).count();
        ;
        dnsduration = (double)p2pReadWriteTime;
        dsduration = dnsduration / ((double)1000000);
        gbpersec = ((2 * iter * bufsize) / dsduration) / ((double)1024 * 1024 * 1024);
        std::cout << " DMA Read Write = " << std::setprecision(2) << std::fixed << gbpersec << "GB/s\n\n";
    }

    clFinish(queue[0]);
    clReleaseMemObject(pbo1);
    clReleaseMemObject(rbo1);
    clReleaseMemObject(pbo2_imported);

    clFinish(queue[1]);
    clReleaseMemObject(pbo2);
    clReleaseMemObject(rbo2);
    clReleaseMemObject(pbo1_imported);
    clReleaseKernel(krnl_dev0);
    clReleaseKernel(krnl_dev1);
    // ------------------------------------------------------------------------------------------------------------------------
    for (uint i = 0; i < 2; i++) {
        clReleaseProgram(program[i]);
        clReleaseContext(context[i]);
        clReleaseCommandQueue(queue[i]);
    }

    std::cout << "Test passed!\n";
    return EXIT_SUCCESS;

} // end of main

// ============================ Helper Functions
// =========================================
static int load_file_to_memory(const char* filename, char** result);
cl_program xcl_import_binary_file(cl_device_id device_id, cl_context context, const char* xclbin_file_name) {
    int err;

    if (access(xclbin_file_name, R_OK) != 0) {
        return nullptr;
        printf("ERROR: %s xclbin not available please build\n", xclbin_file_name);
        exit(EXIT_FAILURE);
    }

    char* krnl_bin;
    const size_t krnl_size = load_file_to_memory(xclbin_file_name, &krnl_bin);

    cl_program program =
        clCreateProgramWithBinary(context, 1, &device_id, &krnl_size, (const unsigned char**)&krnl_bin, nullptr, &err);
    if ((!program) || (err != CL_SUCCESS)) {
        printf("Error: Failed to create compute program from binary %d!\n", err);
        printf("Test failed\n");
        exit(EXIT_FAILURE);
    }

    err = clBuildProgram(program, 0, nullptr, nullptr, nullptr, nullptr);
    if (err != CL_SUCCESS) {
        size_t len;
        char buffer[2048];

        clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
        printf("%s\n", buffer);
        printf("Error: Failed to build program executable!\n");
        exit(EXIT_FAILURE);
    }

    free(krnl_bin);
    return program;
}

static void* smalloc(size_t size);
static int load_file_to_memory(const char* filename, char** result) {
    unsigned int size;

    FILE* f = fopen(filename, "rb");
    if (f == nullptr) {
        *result = nullptr;
        printf("Error: Could not read file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);

    *result = (char*)smalloc(sizeof(char) * (size + 1));

    if (size != fread(*result, sizeof(char), size, f)) {
        free(*result);
        printf("Error: read of kernel failed\n");
        exit(EXIT_FAILURE);
    }

    fclose(f);
    (*result)[size] = 0;

    return size;
}

static void* smalloc(size_t size) {
    void* ptr;

    ptr = malloc(size);

    if (ptr == nullptr) {
        printf("Error: Cannot allocate memory\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}
