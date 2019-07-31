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

/******************************************************************************
 
Description:

    SDx Vector Addition example which highlights the vectorization and
    memory coalescing concept.

*******************************************************************************/

//OpenCL utility layer include
#include "xcl2.hpp"
#include <vector>

//Array size to access
#define DATA_SIZE 8192

//Software implementation of Vector Addition
//Input is of size DATA_SIZE
void softwareGold(int *in1, int *in2, int *out, int size) {
    //Performs Vector Addition out = in1 + in2
    for (int i = 0; i < size; i++) {
        out[i] = in1[i] + in2[i];
    }
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];

    //Allocate Memory in Host Memory
    int size = DATA_SIZE;
    size_t vector_size_bytes = sizeof(int) * DATA_SIZE;
    cl_int err;

    std::vector<int, aligned_allocator<int>> source_in1(vector_size_bytes);
    std::vector<int, aligned_allocator<int>> source_in2(vector_size_bytes);
    std::vector<int, aligned_allocator<int>> source_hw_results(
        vector_size_bytes);
    std::vector<int, aligned_allocator<int>> source_sw_results(
        vector_size_bytes);

    //Create the test data
    for (int i = 0; i < DATA_SIZE; i++) {
        source_in1[i] = i;
        source_in2[i] = i * 2;
        source_hw_results[i] = 0;
        source_sw_results[i] = 0;
    }

    //OPENCL HOST CODE AREA START
    auto devices = xcl::get_xil_devices();
    auto device = devices[0];

    OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(
        err,
        cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
    OCL_CHECK(err,
              std::string device_name = device.getInfo<CL_DEVICE_NAME>(&err));

   auto fileBuf = xcl::read_binary_file(binaryFile);
   cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    devices.resize(1);
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
    OCL_CHECK(err, cl::Kernel kernel(program, "vec_add", &err));

    //Allocate Buffer in Global Memory
    OCL_CHECK(err,
              cl::Buffer buffer_in1(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    vector_size_bytes,
                                    source_in1.data(),
                                    &err));
    OCL_CHECK(err,
              cl::Buffer buffer_in2(context,
                                    CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                    vector_size_bytes,
                                    source_in2.data(),
                                    &err));
    OCL_CHECK(err,
              cl::Buffer buffer_output(context,
                                       CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                                       vector_size_bytes,
                                       source_hw_results.data(),
                                       &err));

    OCL_CHECK(err, err = kernel.setArg(0, buffer_in1));
    OCL_CHECK(err, err = kernel.setArg(1, buffer_in2));
    OCL_CHECK(err, err = kernel.setArg(2, buffer_output));
    OCL_CHECK(err, err = kernel.setArg(3, size));

    //Copy input data to device global memory
    OCL_CHECK(err,
              err = q.enqueueMigrateMemObjects({buffer_in1, buffer_in2},
                                               0 /*0 means from host*/));

    size_t global = 16; // global domain size calculation
    size_t local = 16;  // local domain size calculation

    //Launch the Kernel
    OCL_CHECK(err,
              err =
                  q.enqueueNDRangeKernel(kernel, 0, global, local, NULL, NULL));

    //Copy back the results from device global memory to host memory
    OCL_CHECK(err,
              err = q.enqueueMigrateMemObjects({buffer_output},
                                               CL_MIGRATE_MEM_OBJECT_HOST));
    q.finish();

    //OPENCL HOST CODE AREA END

    //Compute Software Results
    softwareGold(
        source_in1.data(), source_in2.data(), source_sw_results.data(), size);

    //Compare the results of device to the software
    bool match = true;
    for (int i = 0; i < DATA_SIZE; i++) {
        if (source_sw_results[i] != source_hw_results[i]) {
            printf("Mismatch: %d in1 = %d  in2 = %d \t sw_res = %d \t hw_res = "
                   "%d\n",
                   i,
                   source_in1[i],
                   source_in2[i],
                   source_sw_results[i],
                   source_hw_results[i]);
            match = false;
            break;
        }
    }


    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;
    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
