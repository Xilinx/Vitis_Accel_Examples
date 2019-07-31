/**********
Copyright (c) 2018, Xilinx, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/

#include <xcl2.hpp>

#include <cstdio>
#include <fstream>
#include <iosfwd>
#include <string>
#include <vector>

using std::ifstream;
using std::ios;
using std::streamsize;
using std::string;
using std::vector;

#define vendor_name "Xilinx"

#define ERROR_CASE(err)                                                        \
    case err:                                                                  \
        return #err;                                                           \
        break

const char *error_string(cl_int error_code) {
    switch (error_code) {
        ERROR_CASE(CL_SUCCESS);
        ERROR_CASE(CL_DEVICE_NOT_FOUND);
        ERROR_CASE(CL_DEVICE_NOT_AVAILABLE);
        ERROR_CASE(CL_COMPILER_NOT_AVAILABLE);
        ERROR_CASE(CL_MEM_OBJECT_ALLOCATION_FAILURE);
        ERROR_CASE(CL_OUT_OF_RESOURCES);
        ERROR_CASE(CL_OUT_OF_HOST_MEMORY);
        ERROR_CASE(CL_PROFILING_INFO_NOT_AVAILABLE);
        ERROR_CASE(CL_MEM_COPY_OVERLAP);
        ERROR_CASE(CL_IMAGE_FORMAT_MISMATCH);
        ERROR_CASE(CL_IMAGE_FORMAT_NOT_SUPPORTED);
        ERROR_CASE(CL_BUILD_PROGRAM_FAILURE);
        ERROR_CASE(CL_MAP_FAILURE);
        ERROR_CASE(CL_INVALID_VALUE);
        ERROR_CASE(CL_INVALID_DEVICE_TYPE);
        ERROR_CASE(CL_INVALID_PLATFORM);
        ERROR_CASE(CL_INVALID_DEVICE);
        ERROR_CASE(CL_INVALID_CONTEXT);
        ERROR_CASE(CL_INVALID_QUEUE_PROPERTIES);
        ERROR_CASE(CL_INVALID_COMMAND_QUEUE);
        ERROR_CASE(CL_INVALID_HOST_PTR);
        ERROR_CASE(CL_INVALID_MEM_OBJECT);
        ERROR_CASE(CL_INVALID_IMAGE_FORMAT_DESCRIPTOR);
        ERROR_CASE(CL_INVALID_IMAGE_SIZE);
        ERROR_CASE(CL_INVALID_SAMPLER);
        ERROR_CASE(CL_INVALID_BINARY);
        ERROR_CASE(CL_INVALID_BUILD_OPTIONS);
        ERROR_CASE(CL_INVALID_PROGRAM);
        ERROR_CASE(CL_INVALID_PROGRAM_EXECUTABLE);
        ERROR_CASE(CL_INVALID_KERNEL_NAME);
        ERROR_CASE(CL_INVALID_KERNEL_DEFINITION);
        ERROR_CASE(CL_INVALID_KERNEL);
        ERROR_CASE(CL_INVALID_ARG_INDEX);
        ERROR_CASE(CL_INVALID_ARG_VALUE);
        ERROR_CASE(CL_INVALID_ARG_SIZE);
        ERROR_CASE(CL_INVALID_KERNEL_ARGS);
        ERROR_CASE(CL_INVALID_WORK_DIMENSION);
        ERROR_CASE(CL_INVALID_WORK_GROUP_SIZE);
        ERROR_CASE(CL_INVALID_WORK_ITEM_SIZE);
        ERROR_CASE(CL_INVALID_GLOBAL_OFFSET);
        ERROR_CASE(CL_INVALID_EVENT_WAIT_LIST);
        ERROR_CASE(CL_INVALID_EVENT);
        ERROR_CASE(CL_INVALID_OPERATION);
        ERROR_CASE(CL_INVALID_GL_OBJECT);
        ERROR_CASE(CL_INVALID_BUFFER_SIZE);
        ERROR_CASE(CL_INVALID_MIP_LEVEL);
        ERROR_CASE(CL_INVALID_GLOBAL_WORK_SIZE);
        ERROR_CASE(CL_COMPILE_PROGRAM_FAILURE);
        ERROR_CASE(CL_LINKER_NOT_AVAILABLE);
        ERROR_CASE(CL_LINK_PROGRAM_FAILURE);
        ERROR_CASE(CL_DEVICE_PARTITION_FAILED);
        ERROR_CASE(CL_KERNEL_ARG_INFO_NOT_AVAILABLE);
        ERROR_CASE(CL_INVALID_PROPERTY);
        ERROR_CASE(CL_INVALID_IMAGE_DESCRIPTOR);
        ERROR_CASE(CL_INVALID_COMPILER_OPTIONS);
        ERROR_CASE(CL_INVALID_LINKER_OPTIONS);
        ERROR_CASE(CL_INVALID_DEVICE_PARTITION_COUNT);
    default:
        printf("Unknown OpenCL Error (%d)\n", error_code);
        break;
    }
    return nullptr;
}

// This example just prints out string description for given OpenCL error code.
int main(int argc, char **argv) {

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    auto binaryFile = argv[1];

    static const int elements = 1024;
    vector<int, aligned_allocator<int>> A(elements, 32);
    vector<int, aligned_allocator<int>> B(elements, 10);
    vector<int, aligned_allocator<int>> C(elements);

    // Error handling in OpenCL is performed using the cl_int specifier. OpenCL
    // functions either return or accept pointers to cl_int types to indicate if
    // an error occurred.
    cl_int err;

    //OPENCL HOST CODE AREA ENDS
    std::vector<cl::Platform> platforms;
    OCL_CHECK(err, err = cl::Platform::get(&platforms));
    auto num_platforms = platforms.size();
    if (num_platforms == 0) {
        std::cout
            << "No platforms were found. This could be caused because the "
               "OpenCL "
               "icd was not installed in the /etc/OpenCL/vendors directory."
            << std::endl;
        std::cout << "TEST FAILED" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::string platformName;
    cl::Platform platform;
    size_t i = 0;
    for (i = 0; i < platforms.size(); i++) {
        platform = platforms[i];
        OCL_CHECK(err, platformName = platform.getInfo<CL_PLATFORM_NAME>(&err));
        if (platformName == vendor_name) {
            std::cout << "Found Platform" << std::endl;
            std::cout << "Platform Name: " << platformName.c_str() << std::endl;
            break;
        }
    }
    if (i == platforms.size()) {
        std::cout << "Error: Failed to find Xilinx platform" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::vector<cl::Device> devices;
    if ((err = platform.getDevices(CL_DEVICE_TYPE_CPU, &devices))) {
        std::cout
            << "\nRecoverable Error calling cl::Platform::getDevices(): "
            << error_string(err) << std::endl
            << "This error appears when we try to create a device and no "
               "devices "
               "are found on the platform. In this case we passed "
               "CL_DEVICE_TYPE_CPU as the device type which is not available "
               "on "
               "the"
            << platformName.c_str()
            << "platform. We intentionally threw this error so we will not "
               "be exiting the program.\n"
            << std::endl;
    }

    //Getting ACCELERATOR Devices and selecting 1st such device
    OCL_CHECK(err, err = platform.getDevices(CL_DEVICE_TYPE_ALL, &devices));
    auto device = devices[0];

    cl_context_properties props[3] = {
        CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0};

    // cl::Context::Context function call returns the object so they return error codes
    // using pointers to cl_int as their last parameters
    {
        cl::Context context(0, props, NULL, NULL, &err);
        if (err) {
            std::cout << "Recoverable Error calling cl::Context::Context(): "
                      << error_string(err) << std::endl
                      << "\tMost cl::Context* calls accept error codes as "
                         "their last parameter "
                         "instead of returning the error value. This error "
                         "occurred because "
                         "we passed zero(0) for the devices variable. We "
                         "intentionally threw "
                         "this error so we will not be exiting the program.\n"
                      << std::endl;
        }
    }

    std::cout << "Creating Context..." << std::endl;
    OCL_CHECK(err, cl::Context context(device, props, NULL, NULL, &err));
    OCL_CHECK(
        err,
        cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
    OCL_CHECK(err,
              std::string device_name = device.getInfo<CL_DEVICE_NAME>(&err));

    // read_binary_file() is a utility API which will load the binaryFile
    // and will return pointer to file buffer.
   auto fileBuf = xcl::read_binary_file(binaryFile);
   cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    cl::Program::Binaries invalid_bin;
    devices.resize(1);

    {
        cl::Program program(context, devices, invalid_bin, NULL, &err);
        if (err) {
            std::cout << "\nRecoverable Error calling cl::Program::Program(): "
                      << error_string(err) << std::endl
                      << "\tMost cl::Program* calls accept error codes as "
                         "their last parameter "
                         "instead of returning the error value. This error "
                         "occurred because "
                         "we passed an invalid_bin for the bins variable. We "
                         "intentionally threw "
                         "this error so we will not be exiting the program.\n"
                      << std::endl;
        }
    }

    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));

    {
        cl::Kernel kernel(program, "InvalidKernelName", &err);
        if (err) {
            std::cout
                << "Recoverable Error calling cl::Kernel::Kernel(): "
                << error_string(err) << std::endl
                << "Errors calling cl::Kernel are usually caused if the name "
                   "passed into the function does not match a kernel in the "
                   "binary. "
                   "We intentionally caused this error so we will not be "
                   "exiting the "
                   "program.\n"
                << std::endl;
        }
    }

    OCL_CHECK(err, cl::Kernel kernel(program, "vector_add", &err));

    // Allocate Buffer in Global Memory
    // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and
    // Device-to-host communication
    {
        cl::Buffer buffer_a(context, CL_MEM_READ_ONLY, 0, nullptr, &err);
        if (err) {
            std::cout
                << "Recoverable Error calling clCreateBuffer: "
                << error_string(err) << std::endl
                << "There can be several reasons for buffer creation to fail. "
                   "It "
                   "could be because device could not allocate enough memory "
                   "for this "
                   "buffer. The pointer could be null and either "
                   "CL_MEM_USE_HOST_PTR "
                   "or CL_MEM_COPY_HOST_PTR are passed into the flags "
                   "parameter. In "
                   "this case we passed zero(0) as the size of the buffer. We "
                   "intentionally caused this error so we will not be exiting "
                   "the "
                   "program.\n"
                << std::endl;
        }
    }

    size_t size = elements * sizeof(int);
    std::cout << "Creating Buffers..." << std::endl;
    OCL_CHECK(err,
              cl::Buffer buffer_a(context,
                                  CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                  size,
                                  A.data(),
                                  &err));
    OCL_CHECK(err,
              cl::Buffer buffer_b(context,
                                  CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                  size,
                                  B.data(),
                                  &err));
    OCL_CHECK(err,
              cl::Buffer buffer_result(context,
                                       CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                                       size,
                                       C.data(),
                                       &err));

    //Set the Kernel Arguments
    OCL_CHECK(err, err = kernel.setArg(0, buffer_result));
    OCL_CHECK(err, err = kernel.setArg(1, buffer_a));
    OCL_CHECK(err, err = kernel.setArg(2, buffer_b));
    OCL_CHECK(err, err = kernel.setArg(3, elements));

    // Copy input data to device global memory
    std::cout << "Copying data..." << std::endl;
    OCL_CHECK(err,
              err = q.enqueueMigrateMemObjects({buffer_a, buffer_b},
                                               0 /*0 means from host*/));

    // Launch the Kernel
    std::cout << "Launching Kernel..." << std::endl;
    OCL_CHECK(err, err = q.enqueueTask(kernel));

    //Copy Result from Device Global Memory to Host Local Memory
    std::cout << "Getting Results..." << std::endl;
    OCL_CHECK(err,
              err = q.enqueueMigrateMemObjects({buffer_result},
                                               CL_MIGRATE_MEM_OBJECT_HOST));
    OCL_CHECK(err, err = q.finish());
    //OPENCL HOST CODE AREA ENDS

    bool match = true;
    for (int i = 0; i < elements; i++) {
        int host_result = A[i] + B[i];
        if (C[i] != host_result) {
            std::cout << "Error: Result mismatch" << std::endl;
            std::cout << "i = " << i << " CPU result = " << host_result
                      << " Device result = " << C[i] << std::endl;
            match = false;
            break;
        }
    }


    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;
    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
