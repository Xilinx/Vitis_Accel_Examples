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
#include "xcl2.hpp"

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <random>
#include <tuple>
#include <utility>
#include <vector>

using std::default_random_engine;
using std::equal;
using std::generate;
using std::ifstream;
using std::ignore;
using std::make_pair;
using std::numeric_limits;
using std::uniform_int_distribution;
using std::vector;

void find_nearest_neighbor(int *out,
                           const int dim,
                           int *search_points,
                           int *points,
                           const int num_points);

int gen_random() {
    static default_random_engine e;
    static uniform_int_distribution<int> dist(0, 100);

    return dist(e);
}

void verify(vector<int, aligned_allocator<int>> &gold,
            vector<int, aligned_allocator<int>> &test) {
    bool match = true;
    match = equal(begin(gold), end(gold), begin(test));

    if (!match) {
        printf("\n TEST FAILED\n");
        exit(EXIT_FAILURE);
    }
}

void print_point(vector<int, aligned_allocator<int>> &point) {
    for (int element : point)
        printf("%d ", element);
    printf("\n");
}

// This example illustrates the algorithm of nearest neighbor search for a given
// point (x, y) from a list of points.
int main(int argc, char **argv) {

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];

    static const int num_points = 512;
    static const int num_dims = 2;
    cl_int err;

    vector<int, aligned_allocator<int>> data(num_points * num_dims);
    vector<int, aligned_allocator<int>> input(1 * num_dims);
    vector<int, aligned_allocator<int>> out(input.size());
    generate(begin(data), end(data), gen_random);
    generate(begin(input), end(input), gen_random);

    printf("Number Of Points: %d\n", num_points);
    printf("Dimensions: %d\n", num_dims);
    printf("\nInput Point:      ");
    print_point(input);

    vector<int, aligned_allocator<int>> gold(num_dims);
    find_nearest_neighbor(
        gold.data(), num_dims, input.data(), data.data(), num_points);
    printf("Nearest Neighbor: ");
    print_point(gold);

    size_t array_size_bytes = num_points * num_dims * sizeof(int);
    auto devices = xcl::get_xil_devices();
    auto device = devices[0];

    OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(
        err,
        cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
    OCL_CHECK(err,
              std::string device_name = device.getInfo<CL_DEVICE_NAME>(&err));

    //Create Program
   auto fileBuf = xcl::read_binary_file(binaryFile);
   cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    devices.resize(1);
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));

    OCL_CHECK(err,
              cl::Buffer buffer_out(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                                    num_dims * sizeof(int),
                                    out.data(),
                                    &err));
    OCL_CHECK(err,
              cl::Buffer buffer_points(context,
                                       CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                       array_size_bytes,
                                       data.data(),
                                       &err));
    OCL_CHECK(err,
              cl::Buffer buffer_in(context,
                                   CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                   num_dims * sizeof(int),
                                   input.data(),
                                   &err));

    printf("|--------------------------------+-------------------------|\n"
           "| Kernel                         |    Wall-Clock Time (ns) |\n"
           "|--------------------------------+-------------------------|\n");

    // set kernel parameters
    OCL_CHECK(err, cl::Kernel kernel(program, "nearest_neighbor", &err));

    OCL_CHECK(err, err = kernel.setArg(0, buffer_out));
    OCL_CHECK(err, err = kernel.setArg(1, buffer_points));
    OCL_CHECK(err, err = kernel.setArg(2, buffer_in));
    OCL_CHECK(err, err = kernel.setArg(3, num_points));
    OCL_CHECK(err, err = kernel.setArg(4, num_dims));

    // copy the input arrays to memories allocated on the device
    OCL_CHECK(err,
              err = q.enqueueMigrateMemObjects({buffer_points, buffer_in},
                                               0 /* 0 means from host*/));

    cl::Event event;
    uint64_t nstimestart, nstimeend;
    OCL_CHECK(err, err = q.enqueueTask(kernel, NULL, &event));
    q.finish();

    OCL_CHECK(err,
              err = event.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_START,
                                                     &nstimestart));
    OCL_CHECK(err,
              err = event.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_END,
                                                     &nstimeend));
    auto simple_time = nstimeend - nstimestart;
    printf("| %-30s | %23lu |\n", "Nearest Neighbor: simple", simple_time);

    OCL_CHECK(err,
              err = q.enqueueMigrateMemObjects({buffer_out},
                                               CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();
    verify(gold, out);

    // set kernel parameters
    OCL_CHECK(
        err,
        cl::Kernel kernel_loop(program, "nearest_neighbor_loop_fusion", &err));

    OCL_CHECK(err, err = kernel_loop.setArg(0, buffer_out));
    OCL_CHECK(err, err = kernel_loop.setArg(1, buffer_points));
    OCL_CHECK(err, err = kernel_loop.setArg(2, buffer_in));
    OCL_CHECK(err, err = kernel_loop.setArg(3, num_points));
    OCL_CHECK(err, err = kernel_loop.setArg(4, num_dims));

    OCL_CHECK(err, err = q.enqueueTask(kernel_loop, NULL, &event));
    q.finish();
    OCL_CHECK(err,
              err = event.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_START,
                                                     &nstimestart));
    OCL_CHECK(err,
              err = event.getProfilingInfo<uint64_t>(CL_PROFILING_COMMAND_END,
                                                     &nstimeend));
    auto loop_time = nstimeend - nstimestart;
    printf("| %-30s | %23lu |\n", "Nearest Neighbor: loop fusion", loop_time);

    OCL_CHECK(err,
              err = q.enqueueMigrateMemObjects({buffer_out},
                                               CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();

    verify(gold, out);
    printf("|--------------------------------+-------------------------|\n");
    printf("Note: Wall Clock Time is meaningful for real hardware execution "
           "only, not for emulation.\n");
    printf("Please refer to profile summary for kernel execution time for "
           "hardware emulation.\n");
    printf("TEST PASSED\n");
    return EXIT_SUCCESS;
}

void find_nearest_neighbor(int *out,
                           const int dim,
                           int *search_points,
                           int *points,
                           const int num_points) {
    // points is the list of data points that need to be searched for the given
    // point (x, y)
    //
    // output array out has two values - the closest point to given input point
    int best_i = 0;
    int best_dist = numeric_limits<int>::max();

    for (int p = 0; p < num_points; ++p) {
        int dist = 0;

        for (int c = 0; c < dim; ++c) {
            int dx = abs(points[dim * p + c] - search_points[c]);
            dist += dx * dx;
        }

        if (dist < best_dist) {
            best_i = p;
            best_dist = dist;
        }
    }

    for (int c = 0; c < dim; ++c) {
        out[c] = points[best_i * dim + c];
    }
}
