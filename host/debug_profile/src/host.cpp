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
#include "host.h"
#include "experimental/xrt_profile.h"
#include "xcl2.hpp"
#include <vector>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    // Create a user_range using the shortcut constructor.  This will
    //  start measuring the time immediately
    xrt::profile::user_range range("Phase 1", "Start of execution to context creation");

    xrt::profile::user_event events;

    std::string binaryFile = argv[1];

    // Allocate result buffer on host memory
    size_t vector_size_bytes = sizeof(int) * LENGTH;
    std::vector<int, aligned_allocator<int> > source_a(4 * LENGTH);
    std::vector<int, aligned_allocator<int> > result_sim(LENGTH);
    std::vector<int, aligned_allocator<int> > result_krnl(LENGTH);

    // Create the test data and run the vector addition locally
    for (int i = 0; i < LENGTH; i++) {
        source_a[4 * i] = i;
        source_a[4 * i + 1] = 2 * i;
        source_a[4 * i + 2] = 3 * i;
        source_a[4 * i + 3] = 4 * i;
        result_sim[i] = source_a[4 * i] + source_a[4 * i + 1] + source_a[4 * i + 2] + source_a[4 * i + 3];
    }
    range.end();

    events.mark("Test data created");

    range.start("Phase 2", "Context creation and loading of xclbin");

    // OPENCL HOST CODE AREA START
    cl_int err;
    cl::Context context;
    cl::CommandQueue q;
    cl::Kernel krnl;
    auto devices = xcl::get_xil_devices();
    auto fileBuf = xcl::read_binary_file(binaryFile);
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
            OCL_CHECK(err, krnl = cl::Kernel(program, "krnl_vadd", &err));
            valid_device = true;
            break; // we break because we found a valid device
        }
    }
    if (!valid_device) {
        std::cout << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }

    range.end();

    events.mark("Context created and Xclbin loaded");

    range.start("Phase 3", "Kernel and buffer creation");

    OCL_CHECK(err, cl::Buffer buffer_a(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, 4 * vector_size_bytes,
                                       source_a.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_e(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, vector_size_bytes,
                                       result_krnl.data(), &err));

    range.end();

    events.mark("Beffers created");

    range.start("Phase 4", "Setting up arguments and running kernel");

    // Set the kernel arguments
    int vector_length = LENGTH;

    OCL_CHECK(err, err = krnl.setArg(0, buffer_a));
    OCL_CHECK(err, err = krnl.setArg(1, buffer_e));
    OCL_CHECK(err, err = krnl.setArg(2, vector_length));

    // Copy input vectors to memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_a}, 0 /* 0 means from host*/));

    // Launch the kernel and get profile data (stop-start)
    uint64_t nstimestart, nstimeend;
    cl::Event event;
    OCL_CHECK(err, err = q.enqueueTask(krnl, nullptr, &event));
    OCL_CHECK(err, err = q.finish());
    OCL_CHECK(err, err = event.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_START, &nstimestart));
    OCL_CHECK(err, err = event.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_END, &nstimeend));
    auto duration_nanosec = nstimeend - nstimestart;
    std::cout << " **** Duration returned by profile API is " << (duration_nanosec * (1.0e-6)) << " ms **** "
              << std::endl;

    // Copy result to local buffer
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_e}, CL_MIGRATE_MEM_OBJECT_HOST));
    OCL_CHECK(err, err = q.finish());

    range.end();

    events.mark("Kernels finished");

    range.start("Phase 5", "Verification");

    // Compare the results of the kernel to the simulation
    int krnl_match = 0;
    for (int i = 0; i < LENGTH; i++) {
        if (result_sim[i] != result_krnl[i]) {
            std::cout << "Error: Result mismatch" << std::endl;
            std::cout << "i = " << i << " CPU result = " << result_sim[i] << " Krnl Result = " << result_krnl[i]
                      << std::endl;
            krnl_match = 1;
            break;
        } else {
            ; // nothing...Or you could print additional debug message with result
            // matched
        }
    }
    range.end();

    events.mark("Verification done");

    std::cout << "TEST " << (krnl_match ? "FAILED" : "PASSED") << std::endl;
    return (krnl_match ? EXIT_FAILURE : EXIT_SUCCESS);
}
