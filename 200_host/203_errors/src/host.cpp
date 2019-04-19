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

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.h>

#include <cstdio>
#include <fstream>
#include <iosfwd>
#include <string>
#include <vector>

using std::ifstream;
using std::streamsize;
using std::string;
using std::vector;
using std::ios;

#define ERROR_CASE(err) \
    case err:           \
        return #err;    \
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

static const char *error_message =
    "Error: Result mismatch:\n"
    "i = %d CPU result = %d Device result = %d\n";

vector<unsigned char> readBinary(const std::string &fileName) {
    ifstream file(fileName, ios::binary | ios::ate);
    if (file) {
        file.seekg(0, ios::end);
        streamsize size = file.tellg();
        file.seekg(0, ios::beg);
        vector<unsigned char> buffer(size);
        file.read((char *)buffer.data(), size);
        return buffer;
    } else {
        fprintf(stderr, "%s not found\n", fileName.c_str());
        exit(-1);
    }
    return std::vector<unsigned char>(0);
}

// This example just prints out string description for given OpenCL error code.
int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Usage: %s <XCLBIN File>\n", argv[0]);
        return EXIT_FAILURE;
    }
    static const int elements = 1024;
    char *binary_file_path = argv[1];

    // Error handling in OpenCL is performed using the cl_int specifier. OpenCL
    // functions either return or accept pointers to cl_int types to indicate if
    // an error occured.
    cl_int err;

    if ((err = clGetPlatformIDs(0, nullptr, nullptr))) {
        printf(
            "Recoverable Error calling clGetPlatformIDs: %s\n"
            "\tThis error is usually caused by a failed OpenCL installation or "
            "if both the platforms and num_platforms parameters are null. In "
            "this case we passed incorrect values to the function. We "
            "intentionally threw this error so we will not be exiting the "
            "program. \n\n",
            error_string(err));
        // Normally you would exit the program if this call returned an error
        // but in
        // this case we will continue processing after fixing the command
        // exit(EXIT_FAILURE);
    }

    cl_uint num_platforms;
    if ((err = clGetPlatformIDs(0, nullptr, &num_platforms))) {
        printf(
            "Fatal Error calling clGetPlatformIDs: %s\n"
            "This can be caused by an invalid installation of the OpenCL "
            "runtime. Please make sure the installation was successful.\n",
            error_string(err));
        printf("TEST FAILED\n");
        exit(EXIT_FAILURE);
    }

    if (num_platforms == 0) {
        printf(
            "No platforms were found. This could be caused because the OpenCL "
            "icd was not installed in the /etc/OpenCL/vendors directory.\n");
        printf("TEST FAILED\n");
        exit(EXIT_FAILURE);
    }

    vector<cl_platform_id> platforms(num_platforms + 1);
    if ((err = clGetPlatformIDs(num_platforms, platforms.data(), nullptr))) {
        printf("Error: Failed to find an OpenCL platform!\n");
        printf("TEST FAILED\n");
        exit(EXIT_FAILURE);
    }

    string platform_name(1024, '\0');
    size_t actual_size = 0;
    if ((err = clGetPlatformInfo(platforms[0], CL_PLATFORM_NAME,
                                 platform_name.size(),
                                 (void *)platform_name.data(), &actual_size))) {
        printf("Error: Could not determine platform name!\n");
        printf("TEST FAILED\n");
        exit(EXIT_FAILURE);
    }
    printf("Platform Name: %s\n", platform_name.c_str());

    cl_uint num_devices = 0;
    if ((err = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_CPU, 0, nullptr,
                              &num_devices))) {
        printf(
            "Recoverable Error calling clGetDeviceIDs: %s\n"
            "This error appears when we try to create a device and no devices "
            "are found on the platform. In this case we passed "
            "CL_DEVICE_TYPE_CPU as the device type which is not available on "
            "the %s platform. We intentionally threw this error so we will not "
            "be exiting the program.\n\n",
            error_string(err), platform_name.c_str());
    }
    cl_device_id device_id = 0;
    if ((err = clGetDeviceIDs(platforms[0], CL_DEVICE_TYPE_ALL, 1, &device_id,
                              nullptr))) {
        printf(
            "Fatal Error calling clGetDeviceIDs: %s\n"
            "Unexpected error getting device IDs. This may happen if you are "
            "Targeting hardware or software emulation and the "
            "XCL_EMULATION_MODE environment variable is not set. Also makeyou "
            "have set the you have run the emconfigutil to setup the emulation "
            "environment.\n\n",
            error_string(err));
        printf("TEST FAILED\n");
        exit(EXIT_FAILURE);
    }

    cl_context_properties props[3] = {CL_CONTEXT_PLATFORM,
                                      (cl_context_properties)platforms[0], 0};
    // clCreate* function calls return the object so they return error codes
    // using
    // pointers to cl_int as their last parameters
    cl_context context =
        clCreateContext(props, 0, &device_id, nullptr, nullptr, &err);
    if (err) {
        printf(
            "Recoverable Error calling clCreateContext: %s\n"
            "\tMost clCreate* calls accept error codes as their last parameter "
            "instead of returning the error value. This error occured because "
            "we passed 0 for the num_devices varaible. We intentionally threw "
            "this error so we will not be exiting the program.\n\n",
            error_string(err));
    }

    context = clCreateContext(props, 1, &device_id, nullptr, nullptr, &err);

    cl_command_queue command_queue = clCreateCommandQueue(
        context, device_id, CL_QUEUE_PROFILING_ENABLE, &err);
    if (err) {
        printf(
            "Fatal Error calling clCreateCommandQueue: %s\n"
            "Unexpected error creating a command queue.\n\n",
            error_string(err));
        printf("TEST FAILED\n");
        exit(EXIT_FAILURE);
    }

    // Loading the file
    vector<unsigned char> binary = readBinary(binary_file_path);
    size_t binary_size = binary.size();
    size_t incorrect_binary_size = binary.size() - 42;
    const unsigned char *binary_data = binary.data();
    cl_program program = clCreateProgramWithBinary(context, 1, &device_id,
                                                   &incorrect_binary_size,
                                                   &binary_data, NULL, &err);
    if (err) {
        printf(
            "Recoverable Error calling clCreateProgramWithBinary: %s\n"
            "Errors caused during program creation are usually due to invalid "
            "binaries. The binary may be targeting a different device or dsa. "
            "It may also have been currupted or incorrectly read from disk. We "
            "intentionally caused this error so we will not be exiting the "
            "program.\n\n",
            error_string(err));
    }

    program = clCreateProgramWithBinary(context, 1, &device_id, &binary_size,
                                        &binary_data, NULL, &err);
    if (err) {
        printf(
            "Fatal Error calling clCreateProgramWithBinary: %s\n"
            "Unexpected error creating a program from binary. Make sure you "
            "executed this program with the correct path to the binary.\n\n",
            error_string(err));
        printf("TEST FAILED\n");
        exit(EXIT_FAILURE);
    }

    cl_kernel kernel = clCreateKernel(program, "InvalidKernelName", &err);
    if (err) {
        printf(
            "Recoverable Error calling clCreateKernel: %s\n"
            "Errors calling clCreateKernel are usually caused if the name "
            "passed into the function does not match a kernel in the binary. "
            "We intentionally caused this error so we will not be exiting the "
            "program.\n\n",
            error_string(err));
    }

    kernel = clCreateKernel(program, "vector_add", &err);
    if (err) {
        printf(
            "Fatal Error calling clCreateKernel: %s\n"
            "Unexpected error when creating kernel. Make sure you passed the "
            "correct binary into the executable of this program.\n\n",
            error_string(err));
        printf("TEST FAILED\n");
        exit(EXIT_FAILURE);
    }

    cl_mem buffer_a =
        clCreateBuffer(context, CL_MEM_READ_ONLY, 0, nullptr, &err);
    if (err) {
        printf(
            "Recoverable Error calling clCreateBuffer: %s\n"
            "There can be several reasons for buffer creation to fail. It "
            "could be because device could not allocate enough memory for this "
            "buffer. The pointer could be null and either CL_MEM_USE_HOST_PTR "
            "or CL_MEM_COPY_HOST_PTR are passed into the flags parameter. In "
            "this case we passed zero(0) as the size of the buffer. We "
            "intentionally caused this error so we will not be exiting the "
            "program.\n\n",
            error_string(err));
    }

    size_t size = elements * sizeof(int);
    buffer_a = clCreateBuffer(context, CL_MEM_READ_ONLY, size, nullptr, &err);
    if (err) {
        printf("Fatal Error calling clCreateBuffer: %s\n", error_string(err));
        printf("TEST FAILED\n");
        exit(EXIT_FAILURE);
    }
    cl_mem buffer_b =
        clCreateBuffer(context, CL_MEM_READ_ONLY, size, nullptr, &err);
    if (err) {
        printf("Fatal Error calling clCreateBuffer: %s\n", error_string(err));
        printf("TEST FAILED\n");
        exit(EXIT_FAILURE);
    }
    cl_mem buffer_result =
        clCreateBuffer(context, CL_MEM_WRITE_ONLY, size, nullptr, &err);
    if (err) {
        printf("Fatal Error calling clCreateBuffer: %s\n", error_string(err));
        printf("TEST FAILED\n");
        exit(EXIT_FAILURE);
    }

    vector<int> A(elements, 32);
    vector<int> B(elements, 10);
    vector<int> C(elements);

    if ((err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &buffer_result))) {
        printf("Fatal Error calling clSetKernelArg: %s\n", error_string(err));
        printf("TEST FAILED\n");
        exit(EXIT_FAILURE);
    }
    if ((err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &buffer_a))) {
        printf("Fatal Error calling clSetKernelArg: %s\n", error_string(err));
        printf("TEST FAILED\n");
        exit(EXIT_FAILURE);
    }
    if ((err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &buffer_b))) {
        printf("Fatal Error calling clSetKernelArg: %s\n", error_string(err));
        printf("TEST FAILED\n");
        exit(EXIT_FAILURE);
    }

    if ((err = clSetKernelArg(kernel, 3, sizeof(int), &elements))) {
        printf("Fatal Error calling clSetKernelArg: %s\n", error_string(err));
        printf("TEST FAILED\n");
        exit(EXIT_FAILURE);
    }

    if ((err = clEnqueueWriteBuffer(command_queue, buffer_a, CL_FALSE, 0,
                                    size + 1, A.data(), 0, nullptr, nullptr))) {
        printf(
            "Recoverable Error calling clEnqueueWriteBuffer: %s\n"
            "Errors calling clEnqueueWriteBuffer tend to occure due to invalid "
            "pointers or invalid size of the transfer. Make sure that the host "
            "pointer is correct and that you are transferring less than the "
            "size of the buffer. Here we tried to transfer data that was "
            "larger than the size of the buffer. We intentionally caused this "
            "error so we will not be exiting the program.\n\n",
            error_string(err));
    }

    if ((err = clEnqueueWriteBuffer(command_queue, buffer_a, CL_FALSE, 0, size,
                                    A.data(), 0, nullptr, nullptr))) {
        printf("Fatal Error calling clEnqueueWriteBuffer: %s\n",
               error_string(err));
        printf("TEST FAILED\n");
        exit(EXIT_FAILURE);
    }

    if ((err = clEnqueueWriteBuffer(command_queue, buffer_b, CL_FALSE, 0, size,
                                    B.data(), 0, nullptr, nullptr))) {
        printf("Fatal Error calling clEnqueueWriteBuffer: %s\n",
               error_string(err));
        printf("TEST FAILED\n");
        exit(EXIT_FAILURE);
    }

    size_t global = 1;
    size_t local = 1;
    if ((err = clEnqueueNDRangeKernel(command_queue, kernel, 1, nullptr,
                                      &global, &local, 0, nullptr, nullptr))) {
        printf("Fatal Error calling clEnqueueNDRangeKernel: %s\n",
               error_string(err));
        printf("TEST FAILED\n");
        exit(EXIT_FAILURE);
    }

    if ((err = clEnqueueReadBuffer(command_queue, buffer_result, CL_TRUE, 0,
                                   size, C.data(), 0, nullptr, nullptr))) {
        printf("Fatal Error calling clEnqueueWriteBuffer: %s\n",
               error_string(err));
        printf("TEST FAILED\n");
        exit(EXIT_FAILURE);
    }

    int failed = 0;
    for (int i = 0; i < elements; i++) {
        int host_result = A[i] + B[i];
        if (C[i] != host_result) {
            printf(error_message, i, host_result, C[i]);
            failed = 1;
            break;
        }
    }

    clReleaseMemObject(buffer_a);
    clReleaseMemObject(buffer_b);
    clReleaseMemObject(buffer_result);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(command_queue);
    clReleaseContext(context);

    printf("TEST %s\n", (failed ? "FAILED" : "PASSED"));
    return (failed? EXIT_FAILURE :  EXIT_SUCCESS);
}
