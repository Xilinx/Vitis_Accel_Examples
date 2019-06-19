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

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#include "xcl2.hpp"

static const int elements = 256;

void check(cl_int err) {
    if (err) {
        printf("ERROR: Operation Failed: %d\n", err);
        exit(EXIT_FAILURE);
    }
}

void verify(cl::CommandQueue &q, cl::Buffer &buffer, const int value) {
    cl_int err;
    std::vector<int, aligned_allocator<int>> values(elements, 0);
    OCL_CHECK(err,
              err = q.enqueueReadBuffer(buffer,
                                        CL_TRUE,
                                        0,
                                        elements * sizeof(int),
                                        values.data(),
                                        nullptr,
                                        nullptr));

    if (find(begin(values), end(values), value) == end(values)) {
        printf("TEST FAILED\n");
    }
}

// This example illustrates how to transfer data back and forth
// between host and device
int main(int argc, char **argv) {

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];
    cl_int err;
    unsigned fileBufSize;
    std::vector<int, aligned_allocator<int>> host_memory(elements, 42);
    std::vector<int, aligned_allocator<int>> host_memory2(elements, 15);

    size_t size_in_bytes = host_memory.size() * sizeof(int);

    //The get_xil_devices will return vector of Xilinx Devices
    auto devices = xcl::get_xil_devices();
    auto device = devices[0];

    //Creating Context and Command Queue for selected Device
    OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(
        err,
        cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
    std::string device_name = device.getInfo<CL_DEVICE_NAME>();
    printf("Allocating and transferring data to %s\n", device_name.c_str());

    auto fileBuf = xcl::read_binary_file(binaryFile, fileBufSize);
    cl::Program::Binaries bins{{fileBuf, fileBufSize}};
    devices.resize(1);
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));

    //There are several ways to transfer data to the FPGA. The most
    //straightforward way is to transfer the data during the creation of the
    //cl buffer object using the CL_MEM_COPY_HOST_PTR flag
    OCL_CHECK(err,
              cl::Buffer buffer_in(context,
                                   CL_MEM_COPY_HOST_PTR,
                                   size_in_bytes,
                                   host_memory.data(),
                                   &err));

    verify(q, buffer_in, 42);

    // Normally you will be reading and writing to a cl buffer object. The following
    // examples will make use of this buffer object.
    //
    // NOTE: The CL_MEM_READ_ONLY flag indicates that the FPGA can only read from
    // this location. The host can read and write to this buffer at any time
    OCL_CHECK(err,
              cl::Buffer buffer(
                  context, CL_MEM_READ_ONLY, size_in_bytes, nullptr, &err));

    cl::Event blocking_call_event;
    printf("Writing %.1e elements to buffer using a blocking transfer\n",
           (float)elements);

    // enqueueWriteBuffer() API call is a request to enqueue write operation. This API
    // call does not immediately initiate the data transfer. Data transfer happens when
    // a kernel is enqueue which has respective buffer as one of the kernel arguments.
    // So for such kernel call, runtime first transfer the data from host to device
    // and later trigger the kernel call.
    OCL_CHECK(err,
              err = q.enqueueWriteBuffer(
                  buffer,             // buffer on the FPGA
                  CL_TRUE,            // blocking call
                  0,                  // buffer offset in bytes
                  size_in_bytes,      // Size in bytes
                  host_memory.data(), // Pointer to the data to copy
                  nullptr,
                  &blocking_call_event));
    verify(q, buffer, 42);

    cl::Event async_event;
    printf("Writing %.1e elements to buffer using a non-blocking transfer\n",
           (float)elements);

    // Data can also be copied asynchronously with respect to the main thread by
    // sending CL_FALSE as the second parameter
    OCL_CHECK(err,
              err = q.enqueueWriteBuffer(
                  buffer,              // buffer on the FPGA
                  CL_FALSE,            // blocking call
                  0,                   // buffer offset in bytes
                  size_in_bytes,       // Size in bytes
                  host_memory2.data(), // Pointer to the data to copy
                  nullptr,
                  &async_event));
    printf("Write Enqueued. Waiting to complete.\n");
    // It is the user's responsibility to make sure the data does not change
    // between the call and the actual operation. This can be ensured using OpenCL
    // events

    async_event.wait();
    verify(q, buffer, 15);
    printf("Write operation completed successfully\n");

    cl::Event read_event;
    printf("Reading %.1e elements from buffer\n", (float)elements);

    // Data can be transferred back to the host using the read buffer operation
    OCL_CHECK(err,
              err = q.enqueueReadBuffer(
                  buffer,  // This buffers data will be read
                  CL_TRUE, // blocking call
                  0,       // offset
                  size_in_bytes,
                  host_memory.data(), // Data will be stored here
                  nullptr,
                  &read_event));

    printf("Mapping %.1e elements of buffer\n", (float)elements);

    // Mapping and unmapping buffers is another way to transfer memory to and from
    // the FPGA. This operation gives you a pointer that can be freely modified by
    // your host application
    void *ptr = q.enqueueMapBuffer(buffer,       // buffer
                                   CL_TRUE,      // blocking call
                                   CL_MAP_WRITE, // Indicates we will be writing
                                   0,            // buffer offset
                                   size_in_bytes, // size in bytes
                                   nullptr,
                                   nullptr,
                                   &err); // error code
    check(err);

    printf("Modifying elements of buffer\n");

    // You can now assign values to the pointer just like a regular pointer.
    int *data_ptr = reinterpret_cast<int *>(ptr);
    for (int i = 0; i < elements; i++) {
        data_ptr[i] = 33;
    }

    printf("Unmapping elements of buffer\n");

    // The buffer must be unmapped before it can be used in other operations
    OCL_CHECK(err,
              err = q.enqueueUnmapMemObject(buffer,
                                            ptr, // pointer returned by Map call
                                            nullptr,
                                            nullptr));
    verify(q, buffer, 33);

    printf("Using host pointer with MapBuffer\n");

    // You can also instruct OpenCL to use the host pointer using the
    // CL_MEM_USE_HOST_PTR flag. This type of allocation can be useful
    // when the host and the FPGA share the same memory space (e.g. ZYNQ)
    //
    // NOTE: The OpenCL implementation may copy these values into a buffer
    // allocated on the FPGA.
    OCL_CHECK(err,
              cl::Buffer buffer2(context,
                                 CL_MEM_USE_HOST_PTR,
                                 size_in_bytes,
                                 host_memory2.data(),
                                 &err));

    ptr = q.enqueueMapBuffer(buffer2,
                             CL_TRUE,
                             CL_MAP_READ,
                             0,
                             size_in_bytes,
                             nullptr,
                             nullptr,
                             &err);
    check(err);

    OCL_CHECK(err,
              err = q.enqueueUnmapMemObject(buffer2, ptr, nullptr, nullptr));
    verify(q, buffer2, 15);

    // These commands will load vector from the host
    // application and into the buffer2 cl::Buffer objects. The data
    // will be be transferred from system memory over PCIe to the FPGA on-board
    // DDR memory.
    printf("Using host pointer with MigrateMemObjects\n");
    OCL_CHECK(err,
              cl::Buffer buffer_mem(context,
                                    CL_MEM_USE_HOST_PTR,
                                    size_in_bytes,
                                    host_memory2.data(),
                                    &err));
    // clEnqueueMigrateMemObjects does immediate migration of data without considering
    // the fact that data is actually needed or not by kernel operation
    OCL_CHECK(err,
              err = q.enqueueMigrateMemObjects({buffer_mem},
                                               0 /* 0 means from host*/));
    OCL_CHECK(err,
              err = q.enqueueMigrateMemObjects({buffer_mem},
                                               CL_MIGRATE_MEM_OBJECT_HOST));
    OCL_CHECK(err, err = q.finish());

    verify(q, buffer_mem, 15);

    delete[] fileBuf;

    printf("TEST PASSED\n");

    return EXIT_SUCCESS;
}
