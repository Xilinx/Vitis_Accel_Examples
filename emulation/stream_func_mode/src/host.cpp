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

/*******************************************************************************
Description:
    Kernel to kernel streaming example consisting of three compute
    units in a linear hardware pipeline.

    1) Memory read
        This Kernel reads the input vector from Global Memory and streams its
output.

    2) Increment
        This Kernel reads stream input, increments the value and streams to
output.

    3) Memory write
        This Kernel reads from its input stream and writes into Global Memory.

    There are two types of kernel in this example based on
    generated model sources that is Rtl(increment_rtl, mem_read1, mem_write1) and
    functional with systemC wrapper on C(increment_func, mem_read2, mem_write2).
                     ________________
                    |                |<----- Global Memory
                    |   mem_read1    |
                    |________________|------+
                     ________________       | AXI4 Stream
                    |                |<-----+
                    | increment_rtl  |
                    |________________|----->+
                     ________________       | AXI4 Stream
                    |                |<-----+
                    |  mem_write1    |
                    |________________|-----> Global Memory

                     ________________
                    |                |<------ Global Memory
                    |   mem_read2    |
                    |________________|------+
                     ________________       | AXI4 Stream (TLM)
                    |                |<-----+
                    | increment_func |
                    |________________|----->+
                     ________________       | AXI4 Stream (TLM)
                    |                |<-----+
                    |  mem_write2    |
                    |________________|-----> Global Memory



*******************************************************************************/

#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
#include <xcl2.hpp>
#include <iomanip>

void print_summary(std::string k1, std::string k2, double t1, double t2) {
    double speedup = t2 / t1;
    std::cout << "|-------------------------+-------------------------|\n"
              << "| Kernel " << std::right << std::setw(18) << "|" << std::right << std::setw(24)
              << "Wall-Clock Time (s)"
              << " |\n"
              << "|-------------------------+-------------------------|\n";
    std::cout << "| " << std::left << std::setw(24) << k1.c_str() << "|" << std::right << std::setw(24) << t1 << " |\n";
    std::cout << "| " << std::left << std::setw(24) << k2.c_str() << "|" << std::right << std::setw(24) << t2 << " |\n";
    std::cout << "|-------------------------+-------------------------|\n";
    std::cout << "| " << std::left << std::setw(24) << "Speedup:"
              << "|" << std::right << std::setw(24) << speedup << " |\n";
    std::cout << "|-------------------------+-------------------------|\n";
    std::cout << "Note: Wall Clock Time is meaningful for real hardware "
                 "execution only, not for emulation.\n";
    std::cout << "Please refer to profile summary for kernel execution time for "
                 "hardware emulation.\n";
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];

    size_t data_size = 1024 * 1024;
    cl_int err;
    cl::CommandQueue q;
    cl::Context context;
    cl::Kernel krnl_mem_read1, krnl_mem_write1, krnl_mem_read2, krnl_mem_write2;

    // Reducing the data size for emulation mode
    char* xcl_mode = getenv("XCL_EMULATION_MODE");
    if (xcl_mode != nullptr) {
        data_size = 1024;
    }

    // Allocate Memory in Host Memory
    size_t vector_size_bytes = sizeof(int) * data_size;
    std::vector<int, aligned_allocator<int> > source_input(data_size);
    std::vector<int, aligned_allocator<int> > source_hw_results(data_size);
    std::vector<int, aligned_allocator<int> > source_sw_results(data_size);

    // Create the test data and Software Result
    for (size_t i = 0; i < data_size; i++) {
        source_input[i] = i;
        source_sw_results[i] = i + 1;
    }

    // OPENCL HOST CODE AREA START
    // get_xil_devices() is a utility API which will find the Xilinx
    // platforms and will return list of devices connected to Xilinx platform
    std::vector<cl::Device> devices = xcl::get_xil_devices();

    // read_binary_file() is a utility API which will load the binaryFile
    // and will return pointer to file buffer.
    auto fileBuf = xcl::read_binary_file(binaryFile);
    cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    bool valid_device = false;
    for (unsigned int i = 0; i < devices.size(); i++) {
        auto device = devices[i];
        // Creating Context and Command Queue for selected Device
        OCL_CHECK(err, context = cl::Context(device, nullptr, nullptr, nullptr, &err));
        OCL_CHECK(err, q = cl::CommandQueue(context, device,
                                            CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE | CL_QUEUE_PROFILING_ENABLE, &err));

        std::cout << "Trying to program device[" << i << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
        cl::Program program(context, {device}, bins, nullptr, &err);
        if (err != CL_SUCCESS) {
            std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
        } else {
            std::cout << "Device[" << i << "]: program successful!\n";
            OCL_CHECK(err, krnl_mem_read1 = cl::Kernel(program, "mem_read1", &err));
            OCL_CHECK(err, krnl_mem_write1 = cl::Kernel(program, "mem_write1", &err));
            OCL_CHECK(err, krnl_mem_read2 = cl::Kernel(program, "mem_read2", &err));
            OCL_CHECK(err, krnl_mem_write2 = cl::Kernel(program, "mem_write2", &err));
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
    std::cout << "Creating Buffers..." << std::endl;
    OCL_CHECK(err, cl::Buffer buffer_input(context, CL_MEM_READ_WRITE, vector_size_bytes, nullptr, &err));
    OCL_CHECK(err, cl::Buffer buffer_output(context, CL_MEM_READ_WRITE, vector_size_bytes, nullptr, &err));

    // Set the Kernel Arguments
    int size = data_size;
    auto start_rd_incrFunc_wr = std::chrono::high_resolution_clock::now();
    OCL_CHECK(err, err = krnl_mem_read1.setArg(0, buffer_input));
    OCL_CHECK(err, err = krnl_mem_read1.setArg(2, size));
    OCL_CHECK(err, err = krnl_mem_write1.setArg(1, buffer_output));
    OCL_CHECK(err, err = krnl_mem_write1.setArg(2, size));
    // Copy input data to device global memory
    std::cout << "Copying data..." << std::endl;
    OCL_CHECK(err, err = q.enqueueWriteBuffer({buffer_input}, CL_TRUE, 0, vector_size_bytes, source_input.data(),
                                              nullptr, nullptr));

    OCL_CHECK(err, err = q.finish());

    // Launch the Kernel
    std::cout << "Launching Kernels mem_read1 & mem_write1 ..." << std::endl;
    OCL_CHECK(err, err = q.enqueueTask(krnl_mem_read1));
    OCL_CHECK(err, err = q.enqueueTask(krnl_mem_write1));

    // wait for all kernels to finish their operations
    OCL_CHECK(err, err = q.finish());
    auto end_rd_incrFunc_wr = std::chrono::high_resolution_clock::now();

    // Copy Result from Device Global Memory to Host Local Memory
    std::cout << "Getting Results..." << std::endl;
    OCL_CHECK(err, err = q.enqueueReadBuffer({buffer_output}, CL_TRUE, 0, vector_size_bytes, source_hw_results.data(),
                                             nullptr, nullptr));
    OCL_CHECK(err, err = q.finish());
    // OPENCL HOST CODE AREA END

    // Compare the results of the Device to the simulation
    bool match = true;
    for (size_t i = 0; i < data_size; i++) {
        if (source_hw_results[i] != source_sw_results[i]) {
            std::cout << "Error: Result mismatch" << std::endl;
            std::cout << "i = " << i << " CPU result = " << source_sw_results[i]
                      << " Device result = " << source_hw_results[i] << std::endl;
            match = false;
            break;
        }
    }
    auto start_rd_incrRtl_wr = std::chrono::high_resolution_clock::now();
    OCL_CHECK(err, err = krnl_mem_read2.setArg(0, buffer_input));
    OCL_CHECK(err, err = krnl_mem_read2.setArg(2, size));
    OCL_CHECK(err, err = krnl_mem_write2.setArg(1, buffer_output));
    OCL_CHECK(err, err = krnl_mem_write2.setArg(2, size));
    // Copy input data to device global memory
    std::cout << "Copying data..." << std::endl;
    OCL_CHECK(err, err = q.enqueueWriteBuffer({buffer_input}, CL_TRUE, 0, vector_size_bytes, source_input.data(),
                                              nullptr, nullptr));

    OCL_CHECK(err, err = q.finish());

    // Launch the Kernel
    std::cout << "Launching Kernels mem_read2 & mem_write2 ..." << std::endl;
    std::cout << "Launching Kernel..." << std::endl;
    OCL_CHECK(err, err = q.enqueueTask(krnl_mem_read2));
    OCL_CHECK(err, err = q.enqueueTask(krnl_mem_write2));

    // wait for all kernels to finish their operations
    OCL_CHECK(err, err = q.finish());
    auto end_rd_incrRtl_wr = std::chrono::high_resolution_clock::now();
    // Copy Result from Device Global Memory to Host Local Memory
    std::cout << "Getting Results..." << std::endl;
    OCL_CHECK(err, err = q.enqueueReadBuffer({buffer_output}, CL_TRUE, 0, vector_size_bytes, source_hw_results.data(),
                                             nullptr, nullptr));
    OCL_CHECK(err, err = q.finish());
    // OPENCL HOST CODE AREA END

    // Compare the results of the Device to the simulation
    for (size_t i = 0; i < data_size; i++) {
        if (source_hw_results[i] != source_sw_results[i]) {
            std::cout << "Error: Result mismatch" << std::endl;
            std::cout << "i = " << i << " CPU result = " << source_sw_results[i]
                      << " Device result = " << source_hw_results[i] << std::endl;
            match = false;
            break;
        }
    }
    auto elapsed_incrFunc = std::chrono::duration<double>(end_rd_incrFunc_wr - start_rd_incrFunc_wr).count();
    auto elapsed_incrRtl = std::chrono::duration<double>(end_rd_incrRtl_wr - start_rd_incrRtl_wr).count();
    print_summary("read-incrFunc-write", "read-incrRtl-write", elapsed_incrFunc, elapsed_incrRtl);

    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;
    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
