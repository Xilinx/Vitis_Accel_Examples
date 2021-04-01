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
#include <cstdio>
#include <random>
#include <vector>

using std::default_random_engine;
using std::generate;
using std::uniform_int_distribution;
using std::vector;

const int DATA_SIZE = 1 << 10;

int gen_random() {
    static default_random_engine e;
    static uniform_int_distribution<int> dist(0, 100);

    return dist(e);
}

void verify(const vector<int, aligned_allocator<int> >& gold, const vector<int, aligned_allocator<int> >& out) {
    if (!equal(begin(gold), end(gold), begin(out))) {
        printf("TEST FAILED\n");
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];

    // compute the size of array in bytes
    size_t size_in_bytes = DATA_SIZE * sizeof(int);
    cl_int err;
    cl::CommandQueue q;
    cl::Context context;
    cl::Program program;

    // Creates a vector of DATA_SIZE elements with an initial value of 10 and 32
    vector<int, aligned_allocator<int> > source_a(DATA_SIZE);
    vector<int, aligned_allocator<int> > source_b(DATA_SIZE);
    vector<int, aligned_allocator<int> > source_results(DATA_SIZE);
    generate(begin(source_a), end(source_a), gen_random);
    generate(begin(source_b), end(source_b), gen_random);

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

    // Allocate Buffer in Global Memory
    OCL_CHECK(err, cl::Buffer buffer_a(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, size_in_bytes, source_a.data(),
                                       &err));
    OCL_CHECK(err, cl::Buffer buffer_b(context, CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, size_in_bytes, source_b.data(),
                                       &err));
    OCL_CHECK(err, cl::Buffer buffer_result(context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY, size_in_bytes,
                                            source_results.data(), &err));

    vector<int, aligned_allocator<int> > gold(DATA_SIZE);
    transform(begin(source_a), end(source_a), begin(source_b), begin(gold), std::plus<int>());

    printf(
        "|-------------------------+-------------------------|\n"
        "| Kernel                  |    Wall-Clock Time (ns) |\n"
        "|-------------------------+-------------------------|\n");
    OCL_CHECK(err, cl::Kernel kernel_vadd(program, "vadd", &err));

    OCL_CHECK(err, err = kernel_vadd.setArg(0, buffer_result));
    OCL_CHECK(err, err = kernel_vadd.setArg(1, buffer_a));
    OCL_CHECK(err, err = kernel_vadd.setArg(2, buffer_b));
    OCL_CHECK(err, err = kernel_vadd.setArg(3, DATA_SIZE));

    // Copy input data to device global memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_a, buffer_b}, 0 /* 0 means from host*/));

    cl::Event event;
    uint64_t nstimestart, nstimeend;
    OCL_CHECK(err, err = q.enqueueTask(kernel_vadd, nullptr, &event));
    q.finish();
    OCL_CHECK(err, err = event.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_START, &nstimestart));
    OCL_CHECK(err, err = event.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_END, &nstimeend));
    auto simple_time = nstimeend - nstimestart;

    printf("| %-22s  | %23lu |\n", "vadd: simple", simple_time);

    // Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_result}, CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();
    verify(gold, source_results);

    OCL_CHECK(err, cl::Kernel kernel_pipelined(program, "vadd_pipelined", &err));

    OCL_CHECK(err, err = kernel_pipelined.setArg(0, buffer_result));
    OCL_CHECK(err, err = kernel_pipelined.setArg(1, buffer_a));
    OCL_CHECK(err, err = kernel_pipelined.setArg(2, buffer_b));
    OCL_CHECK(err, err = kernel_pipelined.setArg(3, DATA_SIZE));

    OCL_CHECK(err, err = q.enqueueTask(kernel_pipelined, nullptr, &event));
    q.finish();

    OCL_CHECK(err, err = event.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_START, &nstimestart));
    OCL_CHECK(err, err = event.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_END, &nstimeend));
    auto pipelined_time = nstimeend - nstimestart;

    printf("| %-22s  | %23lu |\n", "vadd: pipelined", pipelined_time);
    printf("|-------------------------+-------------------------|\n");
    printf(
        "Note: Wall Clock Time is meaningful for real hardware execution "
        "only, not for emulation.\n");
    printf(
        "Please refer to profile summary for kernel execution time for "
        "hardware emulation.\n");

    // Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_result}, CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();
    verify(gold, source_results);

    printf("TEST PASSED.\n");
    return EXIT_SUCCESS;
}
