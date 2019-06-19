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

/*******************************************************************************
Description: SDx Vector Addition using Blocking Pipes Operation
*******************************************************************************/

#define INCR_VALUE 10

#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
#include <xcl2.hpp>

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    auto binaryFile = argv[1];

    size_t data_size = 1024 * 1024;
    cl_int err;
    unsigned fileBufSize;

    // Reducing the data size for emulation mode
    auto xcl_mode = getenv("XCL_EMULATION_MODE");
    if (xcl_mode != NULL) {
        data_size = 1024;
    }

    //Allocate Memory in Host Memory
    size_t vector_size_bytes = sizeof(int) * data_size;
    std::vector<int, aligned_allocator<int>> source_input(data_size);
    std::vector<int, aligned_allocator<int>> source_hw_results(data_size);
    std::vector<int, aligned_allocator<int>> source_sw_results(data_size);

    // Create the test data and Software Result
    for (size_t i = 0; i < data_size; i++) {
        source_input[i] = i;
        source_sw_results[i] = i + INCR_VALUE;
        source_hw_results[i] = 0;
    }

    //OPENCL HOST CODE AREA START
    // get_xil_devices() is a utility API which will find the Xilinx
    // platforms and will return list of devices connected to Xilinx platform
    auto devices = xcl::get_xil_devices();
    auto device = devices[0];

    std::cout << "Creating Context..." << std::endl;
    OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(err,
              cl::CommandQueue q(context,
                                 device,
                                 CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE |
                                     CL_QUEUE_PROFILING_ENABLE,
                                 &err));
    OCL_CHECK(err,
              std::string device_name = device.getInfo<CL_DEVICE_NAME>(&err));

    // read_binary_file() is a utility API which will load the binaryFile
    // and will return pointer to file buffer.
    auto fileBuf = xcl::read_binary_file(binaryFile, fileBufSize);
    cl::Program::Binaries bins{{fileBuf, fileBufSize}};
    devices.resize(1);
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
    OCL_CHECK(err, cl::Kernel krnl_adder_stage(program, "adder_stage", &err));
    OCL_CHECK(err, cl::Kernel krnl_input_stage(program, "input_stage", &err));
    OCL_CHECK(err, cl::Kernel krnl_output_stage(program, "output_stage", &err));

    // Allocate Buffer in Global Memory
    // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and
    // Device-to-host communication
    std::cout << "Creating Buffers..." << std::endl;
    OCL_CHECK(err,
              cl::Buffer buffer_input(context,
                                      CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                      vector_size_bytes,
                                      source_input.data(),
                                      &err));
    OCL_CHECK(err,
              cl::Buffer buffer_output(context,
                                       CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                                       vector_size_bytes,
                                       source_hw_results.data(),
                                       &err));

    //Set the Kernel Arguments
    int size = data_size;
    int inc = INCR_VALUE;
    OCL_CHECK(err, err = krnl_input_stage.setArg(0, buffer_input));
    OCL_CHECK(err, err = krnl_input_stage.setArg(1, size));
    OCL_CHECK(err, err = krnl_adder_stage.setArg(0, inc));
    OCL_CHECK(err, err = krnl_adder_stage.setArg(1, size));
    OCL_CHECK(err, err = krnl_output_stage.setArg(0, buffer_output));
    OCL_CHECK(err, err = krnl_output_stage.setArg(1, size));

    // Copy input data to device global memory
    std::cout << "Copying data..." << std::endl;
    cl::Event write_event;
    OCL_CHECK(err,
              err = q.enqueueMigrateMemObjects(
                  {buffer_input}, 0 /*0 means from host*/, NULL, &write_event));

    // Launch the Kernel
    std::cout << "Launching Kernel..." << std::endl;
    OCL_CHECK(err, err = q.enqueueTask(krnl_input_stage, NULL, &write_event));
    OCL_CHECK(err, err = q.enqueueTask(krnl_adder_stage, NULL, &write_event));
    OCL_CHECK(err, err = q.enqueueTask(krnl_output_stage, NULL, &write_event));

    //wait for all kernels to finish their operations
    OCL_CHECK(err, err = q.finish());

    //Copy Result from Device Global Memory to Host Local Memory
    std::cout << "Getting Results..." << std::endl;
    OCL_CHECK(err,
              err = q.enqueueMigrateMemObjects({buffer_output},
                                               CL_MIGRATE_MEM_OBJECT_HOST));
    OCL_CHECK(err, err = q.finish());
    //OPENCL HOST CODE AREA END

    // Compare the results of the Device to the simulation
    bool match = true;
    for (size_t i = 0; i < data_size; i++) {
        if (source_hw_results[i] != source_sw_results[i]) {
            std::cout << "Error: Result mismatch" << std::endl;
            std::cout << "i = " << i << " CPU result = " << source_sw_results[i]
                      << " Device result = " << source_hw_results[i]
                      << std::endl;
            match = false;
            break;
        }
    }

    delete[] fileBuf;

    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;
    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
