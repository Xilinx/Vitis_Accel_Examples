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

#include "xcl2.hpp"

#include <array>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>

using std::array;
using std::map;
using std::vector;

cl::Program load_cl2_binary(cl::Program::Binaries, cl::Device device, cl::Context context);
// This example demonstrates how to split work among multiple devices.
int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    auto binaryFile = argv[1];

    cl_int err = CL_SUCCESS;

    // OPENCL HOST CODE AREA START
    // get_xil_devices() is a utility API which will find the Xilinx
    // platforms and will return list of devices connected to Xilinx platform
    auto devices = xcl::get_xil_devices();
    auto device_count = devices.size();

    static const int elements_per_device = xcl::is_hw_emulation() ? (1 << 10) : (1 << 20);
    static const int elements = elements_per_device * device_count;

    vector<int, aligned_allocator<int> > A(elements, 32);
    vector<int, aligned_allocator<int> > B(elements, 10);
    vector<int, aligned_allocator<int> > C(elements);

    // One element per device
    vector<cl::Context> contexts(device_count);
    vector<cl::Program> programs(device_count);
    vector<cl::Kernel> kernels(device_count);
    vector<cl::CommandQueue> queues(device_count);
    vector<std::string> device_name(device_count);

    vector<cl::Buffer> buffer_a(device_count);
    vector<cl::Buffer> buffer_b(device_count);
    vector<cl::Buffer> buffer_result(device_count);
    vector<cl::Program::Binaries> bins(device_count);
    vector<cl::Platform> platform;
    std::vector<unsigned char> fileBuf[device_count];
    OCL_CHECK(err, err = cl::Platform::get(&platform));

    size_t size_per_device = elements_per_device * sizeof(int);
    static const int iter = xcl::is_hw_emulation() ? 2 : 10 * 1024;
    size_t total_size = iter * size_per_device * device_count * 3;
    std::string size_str = xcl::convert_size(total_size);

    cl_context_properties props[3] = {CL_CONTEXT_PLATFORM, (cl_context_properties)(platform[0])(), 0};
    std::cout << "Initializing OpenCL objects" << std::endl;
    for (int d = 0; d < (int)device_count; d++) {
        // In this example. We will create a context for each of the devices
        std::cout << "Creating Context[" << d << "]..." << std::endl;
        OCL_CHECK(err, contexts[d] = cl::Context(devices[d], props, nullptr, nullptr, &err));
        OCL_CHECK(err, queues[d] = cl::CommandQueue(contexts[d], devices[d], CL_QUEUE_PROFILING_ENABLE, &err));
        OCL_CHECK(err, device_name[d] = devices[d].getInfo<CL_DEVICE_NAME>(&err));

        // read_binary_file() ia a utility API which will load the binaryFile
        // and will return pointer to file buffer.
        fileBuf[d] = xcl::read_binary_file(binaryFile);
        bins[d].push_back({fileBuf[d].data(), fileBuf[d].size()});
        programs[d] = load_cl2_binary(bins[d], devices[d], contexts[d]);
        OCL_CHECK(err, kernels[d] = cl::Kernel(programs[d], "vadd", &err));

        // Allocate Buffers in Global Memory
        // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and
        // Device-to-host communication
        size_t offset = d * elements_per_device;
        std::cout << "Creating Buffers[" << d << "]..." << std::endl;
        OCL_CHECK(err, buffer_a[d] = cl::Buffer(contexts[d], CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, size_per_device,
                                                &A[offset], &err));
        OCL_CHECK(err, buffer_b[d] = cl::Buffer(contexts[d], CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, size_per_device,
                                                &B[offset], &err));
        OCL_CHECK(err, buffer_result[d] = cl::Buffer(contexts[d], CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                                                     size_per_device, &C[offset], &err));
    }

    std::chrono::high_resolution_clock::time_point TimeStart = std::chrono::high_resolution_clock::now();
    for (int d = 0; d < (int)device_count; d++) {
        OCL_CHECK(err, err = kernels[d].setArg(0, buffer_result[d]));
        OCL_CHECK(err, err = kernels[d].setArg(1, buffer_a[d]));
        OCL_CHECK(err, err = kernels[d].setArg(2, buffer_b[d]));
        OCL_CHECK(err, err = kernels[d].setArg(3, elements_per_device));
        OCL_CHECK(err, err = kernels[d].setArg(4, iter));

        // Copy input data to device global memory
        std::cout << "Copying data..." << std::endl;
        OCL_CHECK(err, err = queues[d].enqueueMigrateMemObjects({buffer_a[d], buffer_b[d]}, 0 /*0 means from host*/));

        // Launch the Kernel
        std::cout << "Launching Kernel..." << std::endl;
        OCL_CHECK(err, err = queues[d].enqueueTask(kernels[d]));

        // Copy Result from Device Global Memory to Host Local Memory
        std::cout << "Getting Results..." << std::endl;
        OCL_CHECK(err, err = queues[d].enqueueMigrateMemObjects({buffer_result[d]}, CL_MIGRATE_MEM_OBJECT_HOST));
    }

    int dev = 0;
    for (auto queue : queues) {
        std::cout << "Waiting for work to finish on device " << dev++ << std::endl;
        OCL_CHECK(err, err = queue.flush());
        OCL_CHECK(err, err = queue.finish());
    }

    std::chrono::high_resolution_clock::time_point TimeEnd = std::chrono::high_resolution_clock::now();
    double duration_in_ms = std::chrono::duration_cast<std::chrono::microseconds>(TimeEnd - TimeStart).count();

    // OPENCL HOST CODE AREA ENDS
    bool match = true;
    for (int i = 0; i < elements; i++) {
        int host_result = A[i] + B[i];
        if (C[i] != host_result) {
            std::cout << "Error: Result mismatch" << std::endl;
            std::cout << "i = " << i << " CPU result = " << host_result << " Device result = " << C[i] << std::endl;
            match = false;
            break;
        }
    }
    std::cout << "Total Size : " << size_str << std::endl;
    std::cout << "Time Taken : " << duration_in_ms / 1000000 << "sec" << std::endl;
    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;
    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}

cl::Program load_cl2_binary(cl::Program::Binaries bins, cl::Device device, cl::Context context) {
    cl_int err;
    std::vector<cl::Device> devices(1, device);
    OCL_CHECK(err, cl::Program program(context, devices, bins, nullptr, &err));
    return program;
}
