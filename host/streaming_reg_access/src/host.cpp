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
#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <uuid/uuid.h>
#include <vector>
#include <xclhal2.h>
#include "experimental/xclbin_util.h"
// This extension file is required for stream APIs
#include "CL/cl_ext_xilinx.h"
// This file is required for OpenCL C++ wrapper APIs
#include "xcl2.hpp"

// Declaration of custom stream APIs that binds to Xilinx Streaming APIs.
decltype(&clCreateStream) xcl::Stream::createStream = nullptr;
decltype(&clReleaseStream) xcl::Stream::releaseStream = nullptr;
decltype(&clReadStream) xcl::Stream::readStream = nullptr;
decltype(&clWriteStream) xcl::Stream::writeStream = nullptr;
decltype(&clPollStreams) xcl::Stream::pollStreams = nullptr;
decltype(&xclGetComputeUnitInfo) xcl::Ext::getComputeUnitInfo = nullptr;

auto constexpr c_test_size = 4 * 1024; // 4 KB data

////////MAIN FUNCTION//////////
int main(int argc, char** argv) {
    size_t size = c_test_size;
    size_t max_length = 2 * size;
    bool check_status = 0;

    // I/O Data Vectors
    std::vector<int, aligned_allocator<int> > h_data(max_length);
    std::vector<int, aligned_allocator<int> > read_data(size);

    // Command Line Parser
    sda::utils::CmdLineParser parser;

    // Switches
    //**************//"<Full Arg>",  "<Short Arg>", "<Description>", "<Default>"
    parser.addSwitch("--xclbin_file", "-x", "input binary file string", "");
    parser.addSwitch("--register_value", "-i", "regster value to be printed", "");
    parser.parse(argc, argv);

    // Read settings
    auto binaryFile = parser.value("xclbin_file");
    int foo = stoi(parser.value("register_value"));

    if (argc != 5) {
        parser.printHelp();
        return EXIT_FAILURE;
    }

    // Reset the data vector
    for (size_t i = 0; i < max_length; i++) {
        h_data[i] = i + 1;
    }

    // OpenCL Setup
    // OpenCL objects
    cl::Device device;
    cl::Context context;
    cl::CommandQueue q;
    cl::Program program;
    cl::Kernel krnl_incr;

    // Error Status variable
    cl_int err;

    // get_xil_devices() is a utility API which will find the xilinx
    // platforms and will return list of devices connected to Xilinx platform
    auto devices = xcl::get_xil_devices();

    // read_binary_file() is a utility API which will load the binaryFile
    // and will return the pointer to file buffer.
    auto fileBuf = xcl::read_binary_file(binaryFile);
    cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    bool valid_device = false;
    for (unsigned int i = 0; i < devices.size(); i++) {
        device = devices[i];
        // Creating Context and Command Queue for selected Device
        OCL_CHECK(err, context = cl::Context(device, nullptr, nullptr, nullptr, &err));
        OCL_CHECK(err, q = cl::CommandQueue(context, device,
                                            CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &err));

        std::cout << "Trying to program device[" << i << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
        cl::Program program(context, {device}, bins, nullptr, &err);
        if (err != CL_SUCCESS) {
            std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
        } else {
            std::cout << "Device[" << i << "]: program successful!\n";
            // Creating Kernel
            OCL_CHECK(err, krnl_incr = cl::Kernel(program, "increment", &err));

            valid_device = true;
            break; // we break because we found a valid device
        }
    }
    if (!valid_device) {
        std::cerr << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }

    size_t vector_size_bytes = size * sizeof(int);

    auto platform_id = device.getInfo<CL_DEVICE_PLATFORM>(&err);

    // Initialization of streaming class is needed before using it.
    xcl::Stream::init(platform_id);
    xcl::Ext::init(platform_id);

    uuid_t xclbinId;
    xclbin_uuid(fileBuf.data(), xclbinId);

    // Get the device handle
    xclDeviceHandle handle;
    clGetDeviceInfo(device.get(), CL_DEVICE_HANDLE, sizeof(handle), &handle, nullptr);

    cl_uint cuidx;
    xcl::Ext::getComputeUnitInfo(krnl_incr.get(), 0, XCL_COMPUTE_UNIT_INDEX, sizeof(cuidx), &cuidx, nullptr);

    // Get the argument offset
    size_t foo_offset = 0;
    clGetKernelArgInfo(krnl_incr.get(), 2, CL_KERNEL_ARG_OFFSET, sizeof(foo_offset), &foo_offset, nullptr);

    // Use device handle and offset to write the register value
    xclOpenContext(handle, xclbinId, cuidx, false);
    xclRegWrite(handle, cuidx, foo_offset, foo);
    std::cout << "\nThe register value that is written : " << foo << std::endl;
    uint32_t drive_valid = 1;
    xclRegWrite(handle, cuidx, foo_offset + sizeof(int), drive_valid);
    xclCloseContext(handle, xclbinId, cuidx);

    // Streams
    // Device Connection specification of the stream through extension pointer
    cl_int ret;
    cl_mem_ext_ptr_t ext_stream;
    ext_stream.param = krnl_incr.get();
    ext_stream.obj = nullptr;
    ext_stream.flags = 1;

    // Create write stream for argument 0 and 1 of kernel
    cl_stream axis00_stream, axis01_stream;
    OCL_CHECK(ret, axis00_stream =
                       xcl::Stream::createStream(device.get(), XCL_STREAM_WRITE_ONLY, CL_STREAM, &ext_stream, &ret));

    ext_stream.flags = 0;
    OCL_CHECK(ret, axis01_stream =
                       xcl::Stream::createStream(device.get(), XCL_STREAM_READ_ONLY, CL_STREAM, &ext_stream, &ret));

    // Initiating the WRITE transfer
    cl_stream_xfer_req wr_req{0};

    wr_req.flags = CL_STREAM_EOT;

    OCL_CHECK(ret, xcl::Stream::writeStream(axis01_stream, h_data.data(), vector_size_bytes, &wr_req, &ret));

    // Initiating the READ transfer
    cl_stream_xfer_req rd_req{0};
    rd_req.flags = CL_STREAM_EOT;
    OCL_CHECK(ret, xcl::Stream::readStream(axis00_stream, read_data.data(), vector_size_bytes, &rd_req, &ret));

    uint32_t beyond_foo;
    size_t beyond_foo_offset = 0;
    clGetKernelArgInfo(krnl_incr.get(), 3, CL_KERNEL_ARG_OFFSET, sizeof(beyond_foo_offset), &beyond_foo_offset,
                       nullptr);

    // Use device handle and offset to read the register value
    xclOpenContext(handle, xclbinId, cuidx, false);
    xclRegRead(handle, cuidx, beyond_foo_offset, &beyond_foo);
    std::cout << "\nThe register value that is read : " << beyond_foo << std::endl;
    xclCloseContext(handle, xclbinId, cuidx);

    for (cl_uint i = 0; i < size; i++) {
        if (h_data[i] != read_data[i]) {
            check_status = 1;
        }
    }

    int expected_result = size - foo;

    std::cout << "\nThe number of data should more than " << foo << " is " << expected_result;
    std::cout << "\nThe Result coming from hardware is " << beyond_foo << std::endl;

    if (expected_result == (int)beyond_foo)
        std::cout << "\nTEST PASSED\n";
    else
        std::cout << "\nTEST FAILED\n";

    // Ensuring all OpenCL objects are released.
    q.finish();

    // Releasing Streams
    xcl::Stream::releaseStream(axis00_stream);
    xcl::Stream::releaseStream(axis01_stream);
    return (check_status ? EXIT_FAILURE : EXIT_SUCCESS);
}
