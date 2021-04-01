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
#include <iostream>
#include <random>
#include <stdint.h>
#include <vector>

constexpr int DATA_SIZE = 1024;

// Compare the results of the Device to the simulation
void verify(const std::vector<uint64_t, aligned_allocator<uint64_t> >& gold,
            const std::vector<uint64_t, aligned_allocator<uint64_t> >& out) {
    bool match = true;
    for (int i = 0; i < DATA_SIZE; i++) {
        if (out[i] != gold[i]) {
            std::cout << "Error: Result mismatch" << std::endl;
            std::cout << "i = " << i << " CPU result = " << gold[i] << " Device result = " << out[i] << std::endl;
            match = false;
            break;
        }
    }
    if (!match) {
        std::cout << "TEST FAILED" << std::endl;
        exit(EXIT_FAILURE);
    }
}

void loop_sequential_sw(const std::vector<uint32_t, aligned_allocator<uint32_t> >& A,
                        const std::vector<uint32_t, aligned_allocator<uint32_t> >& B,
                        std::vector<uint64_t, aligned_allocator<uint64_t> >& RES) {
    for (int i = 0; i < DATA_SIZE; i++) {
        RES[i] = A[i] * B[i];
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];

    // compute the size of array in bytes
    size_t input_size_in_bytes = DATA_SIZE * sizeof(uint32_t);
    size_t output_size_in_bytes = DATA_SIZE * sizeof(uint64_t);
    cl_int err;
    cl::CommandQueue q;
    cl::Context context;
    cl::Program program;

    /* less repeatitions for emulation mode */
    int reps = xcl::is_emulation() ? 2 : 1000000;

    std::vector<uint32_t, aligned_allocator<uint32_t> > source_a(DATA_SIZE);
    std::vector<uint32_t, aligned_allocator<uint32_t> > source_b(DATA_SIZE);
    std::vector<uint64_t, aligned_allocator<uint64_t> > source_sw_results(DATA_SIZE);
    std::vector<uint32_t, aligned_allocator<uint32_t> > source_a_sw(DATA_SIZE);
    std::vector<uint32_t, aligned_allocator<uint32_t> > source_b_sw(DATA_SIZE);
    std::vector<uint64_t, aligned_allocator<uint64_t> > source_hw_results(DATA_SIZE);

    // Create the test data
    std::generate(source_a.begin(), source_a.end(), std::rand);
    std::generate(source_b.begin(), source_b.end(), std::rand);
    for (int i = 0; i < DATA_SIZE; i++) {
        source_a_sw[i] = source_a[i];
        source_b_sw[i] = source_b[i];
        source_sw_results[i] = 0;
        source_hw_results[i] = 0;
    }

    // Getting Xilinx Device and creating program and context
    auto devices = xcl::get_xil_devices();
    // read_binary_file() is a utility API which will load the binaryFile
    // and will return the pointer to file buffer.
    auto fileBuf = xcl::read_binary_file(binaryFile);
    cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    bool valid_device = false;
    for (unsigned int i = 0; i < devices.size(); i++) {
        auto device = devices[i];
        // Creating Context and Command Queue for selected Device
        OCL_CHECK(err, context = cl::Context(device, nullptr, nullptr, nullptr, &err));
        OCL_CHECK(err, q = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err));

        std::cout << "Trying to program device[" << i << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
        program = cl::Program(context, {device}, bins, nullptr, &err);
        if (err != CL_SUCCESS) {
            std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
        } else {
            std::cout << "Device[" << i << "]: program successful!\n";
            valid_device = true;
            break; // we break because we found a valid device
        }
    }
    if (!valid_device) {
        std::cout << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }

    loop_sequential_sw(source_a, source_b, source_sw_results);

    // Allocate Buffer in Global Memory
    OCL_CHECK(err, cl::Buffer buffer_a(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, input_size_in_bytes,
                                       source_a.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_b(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, input_size_in_bytes,
                                       source_b.data(), &err));
    OCL_CHECK(err, cl::Buffer buffer_result(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, output_size_in_bytes,
                                            source_hw_results.data(), &err));

    OCL_CHECK(err, cl::Kernel dot_product_1(program, "dot_product_1", &err));

    OCL_CHECK(err, err = dot_product_1.setArg(0, buffer_a));
    OCL_CHECK(err, err = dot_product_1.setArg(1, buffer_b));
    OCL_CHECK(err, err = dot_product_1.setArg(2, buffer_result));
    OCL_CHECK(err, err = dot_product_1.setArg(3, DATA_SIZE));
    OCL_CHECK(err, err = dot_product_1.setArg(4, reps));

    // Copy input data to device global memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_a, buffer_b}, 0 /* 0 means from host*/));
    auto total_start = std::chrono::high_resolution_clock::now();
    OCL_CHECK(err, err = q.enqueueTask(dot_product_1));
    q.finish();
    auto total_end = std::chrono::high_resolution_clock::now();
    auto dot_product_1_time_ns = std::chrono::duration<double, std::nano>(total_end - total_start);
    // Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_result}, CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();
    verify(source_sw_results, source_hw_results);

    OCL_CHECK(err, cl::Kernel dot_product_2(program, "dot_product_2", &err));

    OCL_CHECK(err, err = dot_product_2.setArg(0, buffer_a));
    OCL_CHECK(err, err = dot_product_2.setArg(1, buffer_b));
    OCL_CHECK(err, err = dot_product_2.setArg(2, buffer_result));
    OCL_CHECK(err, err = dot_product_2.setArg(3, DATA_SIZE));
    OCL_CHECK(err, err = dot_product_2.setArg(4, reps));

    total_start = std::chrono::high_resolution_clock::now();
    OCL_CHECK(err, err = q.enqueueTask(dot_product_2));
    q.finish();
    total_end = std::chrono::high_resolution_clock::now();
    auto dot_product_2_time_ns = std::chrono::duration<double, std::nano>(total_end - total_start);
    // Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_result}, CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();
    verify(source_sw_results, source_hw_results);

    OCL_CHECK(err, cl::Kernel dot_product_3(program, "dot_product_3", &err));

    OCL_CHECK(err, err = dot_product_3.setArg(0, buffer_a));
    OCL_CHECK(err, err = dot_product_3.setArg(1, buffer_b));
    OCL_CHECK(err, err = dot_product_3.setArg(2, buffer_result));
    OCL_CHECK(err, err = dot_product_3.setArg(3, DATA_SIZE));
    OCL_CHECK(err, err = dot_product_3.setArg(4, reps));

    total_start = std::chrono::high_resolution_clock::now();
    OCL_CHECK(err, err = q.enqueueTask(dot_product_3));
    q.finish();
    total_end = std::chrono::high_resolution_clock::now();
    auto dot_product_3_time_ns = std::chrono::duration<double, std::nano>(total_end - total_start);
    // Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_result}, CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();
    verify(source_sw_results, source_hw_results);

    OCL_CHECK(err, cl::Kernel dot_product_4(program, "dot_product_4", &err));

    OCL_CHECK(err, err = dot_product_4.setArg(0, buffer_a));
    OCL_CHECK(err, err = dot_product_4.setArg(1, buffer_b));
    OCL_CHECK(err, err = dot_product_4.setArg(2, buffer_result));
    OCL_CHECK(err, err = dot_product_4.setArg(3, DATA_SIZE));
    OCL_CHECK(err, err = dot_product_4.setArg(4, reps));

    total_start = std::chrono::high_resolution_clock::now();
    OCL_CHECK(err, err = q.enqueueTask(dot_product_4));
    q.finish();
    total_end = std::chrono::high_resolution_clock::now();
    auto dot_product_4_time_ns = std::chrono::duration<double, std::nano>(total_end - total_start);
    // Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_result}, CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();
    verify(source_sw_results, source_hw_results);

    OCL_CHECK(err, cl::Kernel dot_product_5(program, "dot_product_5", &err));

    OCL_CHECK(err, err = dot_product_5.setArg(0, buffer_a));
    OCL_CHECK(err, err = dot_product_5.setArg(1, buffer_b));
    OCL_CHECK(err, err = dot_product_5.setArg(2, buffer_result));
    OCL_CHECK(err, err = dot_product_5.setArg(3, DATA_SIZE));
    OCL_CHECK(err, err = dot_product_5.setArg(4, reps));

    total_start = std::chrono::high_resolution_clock::now();
    OCL_CHECK(err, err = q.enqueueTask(dot_product_5));
    q.finish();
    total_end = std::chrono::high_resolution_clock::now();
    auto dot_product_5_time_ns = std::chrono::duration<double, std::nano>(total_end - total_start);
    // Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_result}, CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();
    verify(source_sw_results, source_hw_results);

    printf(
        "|-------------------------+-----------------------|\n"
        "| Kernel(%3d repeatitions)|  Wall-Clock Time (sec) |\n"
        "|-------------------------+-----------------------|\n",
        reps);
    std::cout << "| dot_product_1\t\t  |\t" << dot_product_1_time_ns.count() / 1000000000 << "\t  |" << std::endl;
    std::cout << "| dot_product_2\t\t  |\t" << dot_product_2_time_ns.count() / 1000000000 << "\t  |" << std::endl;
    std::cout << "| dot_product_3\t\t  |\t" << dot_product_3_time_ns.count() / 1000000000 << "\t  |" << std::endl;
    std::cout << "| dot_product_4\t\t  |\t" << dot_product_4_time_ns.count() / 1000000000 << "\t  |" << std::endl;
    std::cout << "| dot_product_5\t\t  |\t" << dot_product_5_time_ns.count() / 1000000000 << "\t  |" << std::endl;
    std::cout << "|-------------------------+-----------------------|" << std::endl;
    std::cout << "Note: Wall Clock Time is meaningful for real hardware execution "
                 "only, not for emulation."
              << std::endl;
    std::cout << "TEST PASSED" << std::endl;
    return EXIT_SUCCESS;
}
