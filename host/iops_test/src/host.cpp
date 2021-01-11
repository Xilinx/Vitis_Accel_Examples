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

#include "xcl2.hpp"
#include <algorithm>
#include <chrono>
#include <vector>

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];

    int iter = xcl::is_emulation() ? 100 : 1000000;

    cl_int err;
    cl::Context context;
    cl::Kernel krnl_cntr;
    cl::CommandQueue q;

    std::vector<int, aligned_allocator<int> > source_hw_results(1);

    source_hw_results[0] = 0;

    // OPENCL HOST CODE AREA START
    // get_xil_devices() is a utility API which will find the xilinx
    // platforms and will return list of devices connected to Xilinx platform
    auto devices = xcl::get_xil_devices();
    // read_binary_file() is a utility API which will load the binaryFile
    // and will return the pointer to file buffer.
    auto fileBuf = xcl::read_binary_file(binaryFile);
    cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    bool valid_device = false;
    for (unsigned int i = 0; i < devices.size(); i++) {
        auto device = devices[i];
        // Creating Context and Command Queue for selected Device
        OCL_CHECK(err, context = cl::Context(device, NULL, NULL, NULL, &err));
        OCL_CHECK(err, q = cl::CommandQueue(context, device,
                                            CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &err));
        std::cout << "Trying to program device[" << i << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
        cl::Program program(context, {device}, bins, NULL, &err);
        if (err != CL_SUCCESS) {
            std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
        } else {
            std::cout << "Device[" << i << "]: program successful!\n";
            OCL_CHECK(err, krnl_cntr = cl::Kernel(program, "krnl_cntr", &err));
            valid_device = true;
            break; // we break because we found a valid device
        }
    }
    if (!valid_device) {
        std::cout << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }

    // Allocate Buffer in Global Memory
    // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and
    // Device-to-host communication
    OCL_CHECK(err, cl::Buffer buffer_a(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE, sizeof(int),
                                       source_hw_results.data(), &err));

    // reset execution counter to 0
    int reset = 1;
    OCL_CHECK(err, err = krnl_cntr.setArg(0, buffer_a));
    OCL_CHECK(err, err = krnl_cntr.setArg(1, reset));

    // Copy input data to device global memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_a}, 0 /* 0 means from host*/));

    OCL_CHECK(err, err = q.enqueueTask(krnl_cntr));
    q.finish();

    reset = 0;
    OCL_CHECK(err, err = krnl_cntr.setArg(1, reset));

    std::chrono::high_resolution_clock::time_point TimeStart = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iter; i++) {
        OCL_CHECK(err, err = q.enqueueTask(krnl_cntr));
    }
    q.finish();
    std::chrono::high_resolution_clock::time_point TimeEnd = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration_cast<std::chrono::microseconds>(TimeEnd - TimeStart).count();

    // Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_a}, CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();

    // Compare the results of the Device to the simulation
    bool match = true;
    if (source_hw_results[0] != iter) {
        std::cout << "Error: Result mismatch" << std::endl;
        std::cout << "Expected result = " << iter << " Device result = " << source_hw_results[0] << std::endl;
        match = false;
    }

    std::cout << "Time for " << iter << " enqueues: " << duration / 1000000 << " sec" << std::endl;
    std::cout << "Performance : " << iter / (duration / 1000000) << " IOPS" << std::endl;
    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;
    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
