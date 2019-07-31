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

/*
 Out of order kernel execution

 Command queues are an OpenCL construct that manages the execution of memory
 transfers and kernel execution. Tasks are inserted into the queue using
 clEnqueue* operations. The device executes these operations in the order they
 are inserted into the queue.

 One of the many advantages of an FPGA is that all kernels are implemented in
 dedicated hardware. These regions can execute two different tasks in parallel
 without effecting the performance or resources of the other operations.

            +------------------------------+
            |           Xilinx FPGA        |
            |  +----------+  +----------+  |
            |  |          |  |          |  |
            |  | kernel 1 |  | kernel 2 |  |
            |  |          |  |          |  |
            |  |          |  |          |  |
            |  +----------+  +----------+  |
            |       ↑             ↑        |
            +-------|-------------|--------+
                    |             |
                 Task 1        Task 2

 An application can take advantage of this feature of the FPGA by creating
 multiple command queues. Independent tasks can be scheduled into different
 command queues which can be executed on the FPGA as the modules become
 available.

 An application can also create out of order command queues to execute tasks in
 parallel. Tasks queued on an out of order command queue can be executed in any
 order by the OpenCL implementation. Out of order queues are created by passing
 the CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE to the clCreateCommandQueue
 function. These types of queues executes tasks as resources become available at
 runtime.

 Events objects can be used to synchronize command queues or tasks in an out of
 order command queue. See the events example for additional details regarding
 the proper use of events in OpenCL.

 This example will demonstrate how to execute multiple kernels in parallel on an
 FPGA. We will create three kernels that will perform scaling, matrix addition
 and matrix multiplication. The Matrix multiplication is independent of the
 scaling and addition operation so we will execute independently of the other
 kernels.

 The following operations will be performed:

  A = 2 * A (scale A by 2)
  C = A + B (Add A and B)
  F = D * E (Matrix multiplication on D and E)

 NOTE: Normally calculating the C matrix can be performed by one kernel. We are
 splitting this task in the examples to illustrate the execution concepts.

  - Task flow (1): Matrix Scaling <------ Matrix Addition
  - Task flow (2): Matrix Multiplication
 */

#include "xcl2.hpp"

#include <algorithm>
#include <cstdio>
#include <string>
#include <vector>

using std::find;
using std::vector;

const int MAT_DIM0 = 16;
const int MAT_DIM1 = 16;
size_t offset = 0;
size_t global = 1;
size_t local = 1;

// An event callback function that prints the operations performed by the OpenCL
// runtime.

void event_cb(cl_event event1, cl_int cmd_status, void *data) {
    cl_int err;
    cl_command_type command;
    cl::Event event(event1, true);
    OCL_CHECK(
        err,
        err = event.getInfo<cl_command_type>(CL_EVENT_COMMAND_TYPE, &command));
    cl_int status;
    OCL_CHECK(err,
              err = event.getInfo<cl_int>(CL_EVENT_COMMAND_EXECUTION_STATUS,
                                          &status));

    const char *command_str;
    const char *status_str;
    switch (command) {
    case CL_COMMAND_READ_BUFFER:
        command_str = "buffer read";
        break;
    case CL_COMMAND_WRITE_BUFFER:
        command_str = "buffer write";
        break;
    case CL_COMMAND_NDRANGE_KERNEL:
        command_str = "kernel";
        break;
    }
    switch (status) {
    case CL_QUEUED:
        status_str = "Queued";
        break;
    case CL_SUBMITTED:
        status_str = "Submitted";
        break;
    case CL_RUNNING:
        status_str = "Executing";
        break;
    case CL_COMPLETE:
        status_str = "Completed";
        break;
    }
    printf(
        "%s %s %s\n", status_str, reinterpret_cast<char *>(data), command_str);
    fflush(stdout);
}

// Sets the callback for a particular event
void set_callback(cl::Event event, const char *queue_name) {
    cl_int err;
    OCL_CHECK(err,
              err =
                  event.setCallback(CL_COMPLETE, event_cb, (void *)queue_name));
}

// Verify the result of the out put buffers
void verify_results(const vector<int> &C, const vector<int> &F) {
    if (find(begin(C), end(C), 0) != end(C)) {
        printf("\nC = 2*A + B computation FAILED\n");
        exit(EXIT_FAILURE);
    }
    if (find(begin(F), end(F), 0) != end(F)) {
        printf("\nF = matmul(D, E) computation FAILED\n");
        exit(EXIT_FAILURE);
    }
}

void multiple_command_queues(cl::Context &context,
                             cl::Device &device,
                             cl::Kernel &kernel_mscale,
                             cl::Kernel &kernel_madd,
                             cl::Kernel &kernel_mmult,
                             cl::Buffer &buffer_a,
                             cl::Buffer &buffer_b,
                             cl::Buffer &buffer_c,
                             cl::Buffer &buffer_d,
                             cl::Buffer &buffer_e,
                             cl::Buffer &buffer_f,
                             size_t size_in_bytes) {
    cl_int err;
    OCL_CHECK(err,
              cl::CommandQueue ordered_queue1(
                  context, device, CL_QUEUE_PROFILING_ENABLE, &err));
    OCL_CHECK(err,
              cl::CommandQueue ordered_queue2(
                  context, device, CL_QUEUE_PROFILING_ENABLE, &err));

    // copy the input arrays to input memory allocated on the accelerator
    // devices
    const int matrix_scale_factor = 2;
    OCL_CHECK(err, err = kernel_mscale.setArg(0, buffer_a));
    OCL_CHECK(err, err = kernel_mscale.setArg(1, matrix_scale_factor));
    OCL_CHECK(err, err = kernel_mscale.setArg(2, MAT_DIM0));
    OCL_CHECK(err, err = kernel_mscale.setArg(3, MAT_DIM1));

    vector<cl::Event> kernel_events(3);

    printf("[Ordered Queue 1]: Enqueueing scale kernel\n");
    OCL_CHECK(
        err,
        err = ordered_queue1.enqueueNDRangeKernel(
            kernel_mscale, offset, global, local, nullptr, &kernel_events[0]));

    set_callback(kernel_events[0], "scale");

    OCL_CHECK(err, err = kernel_madd.setArg(0, buffer_c));
    OCL_CHECK(err, err = kernel_madd.setArg(1, buffer_a));
    OCL_CHECK(err, err = kernel_madd.setArg(2, buffer_b));
    OCL_CHECK(err, err = kernel_madd.setArg(3, MAT_DIM0));
    OCL_CHECK(err, err = kernel_madd.setArg(4, MAT_DIM1));

    printf("[Ordered Queue 1]: Enqueueing addition kernel\n");
    OCL_CHECK(
        err,
        err = ordered_queue1.enqueueNDRangeKernel(
            kernel_madd, offset, global, local, nullptr, &kernel_events[1]));

    set_callback(kernel_events[1], "addition");

    // set OpenCL kernel parameters to multiply matrix D and E */
    OCL_CHECK(err, err = kernel_mmult.setArg(0, buffer_f));
    OCL_CHECK(err, err = kernel_mmult.setArg(1, buffer_d));
    OCL_CHECK(err, err = kernel_mmult.setArg(2, buffer_e));
    OCL_CHECK(err, err = kernel_mmult.setArg(3, MAT_DIM0));
    OCL_CHECK(err, err = kernel_mmult.setArg(4, MAT_DIM1));

    printf("[Ordered Queue 2]: Enqueueing matrix multiplication kernel\n");
    OCL_CHECK(
        err,
        err = ordered_queue2.enqueueNDRangeKernel(
            kernel_mmult, offset, global, local, nullptr, &kernel_events[2]));
    set_callback(kernel_events[2], "matrix multiplication");

    const size_t array_size = MAT_DIM0 * MAT_DIM1;
    vector<int> A(array_size);
    vector<int> C(array_size);
    vector<int> F(array_size);

    vector<cl::Event> transfer_events(3);
    printf("[Ordered Queue 1]: Enqueueing Read Buffer A\n");
    OCL_CHECK(err,
              err = ordered_queue1.enqueueReadBuffer(buffer_a,
                                                     CL_FALSE,
                                                     0,
                                                     size_in_bytes,
                                                     A.data(),
                                                     nullptr,
                                                     &transfer_events[0]));
    set_callback(transfer_events[0], "A");

    printf("[Ordered Queue 1]: Enqueueing Read Buffer C\n");
    OCL_CHECK(err,
              err = ordered_queue1.enqueueReadBuffer(buffer_c,
                                                     CL_FALSE,
                                                     0,
                                                     size_in_bytes,
                                                     C.data(),
                                                     nullptr,
                                                     &transfer_events[1]));
    set_callback(transfer_events[1], "C");

    printf("[Ordered Queue 2]: Enqueueing Read Buffer F\n");
    OCL_CHECK(err,
              err = ordered_queue2.enqueueReadBuffer(buffer_f,
                                                     CL_FALSE,
                                                     0,
                                                     size_in_bytes,
                                                     F.data(),
                                                     nullptr,
                                                     &transfer_events[2]));
    set_callback(transfer_events[2], "F");

    printf("[Ordered Queue 1]: Waiting\n");
    printf("[Ordered Queue 2]: Waiting\n");
    OCL_CHECK(err, err = cl::Event::waitForEvents(transfer_events));
    verify_results(C, F);
}

void out_of_order_queue(cl::Context &context,
                        cl::Device &device,
                        cl::Kernel &kernel_mscale,
                        cl::Kernel &kernel_madd,
                        cl::Kernel &kernel_mmult,
                        cl::Buffer &buffer_a,
                        cl::Buffer &buffer_b,
                        cl::Buffer &buffer_c,
                        cl::Buffer &buffer_d,
                        cl::Buffer &buffer_e,
                        cl::Buffer &buffer_f,
                        size_t size_in_bytes) {
    cl_int err;
    vector<cl::Event> ooo_events(6);
    vector<cl::Event> kernel_wait_events;

    // We are creating an out of order queue here.
    OCL_CHECK(
        err,
        cl::CommandQueue ooo_queue(context,
                                   device,
                                   CL_QUEUE_PROFILING_ENABLE |
                                       CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE,
                                   &err));

    // Clear values in the result buffers
    {
        int zero = 0;
        int one = 1;
        vector<cl::Event> fill_events(3);
        OCL_CHECK(
            err,
            err = ooo_queue.enqueueFillBuffer(
                buffer_a, one, 0, size_in_bytes, nullptr, &fill_events[0]));
        OCL_CHECK(
            err,
            err = ooo_queue.enqueueFillBuffer(
                buffer_c, zero, 0, size_in_bytes, nullptr, &fill_events[1]));
        OCL_CHECK(
            err,
            err = ooo_queue.enqueueFillBuffer(
                buffer_f, zero, 0, size_in_bytes, nullptr, &fill_events[2]));
        OCL_CHECK(err, err = cl::Event::waitForEvents(fill_events));
    }

    // copy the input arrays to input memory allocated on the accelerator
    // devices
    const int matrix_scale_factor = 2;
    OCL_CHECK(err, err = kernel_mscale.setArg(0, buffer_a));
    OCL_CHECK(err, err = kernel_mscale.setArg(1, matrix_scale_factor));
    OCL_CHECK(err, err = kernel_mscale.setArg(2, MAT_DIM0));
    OCL_CHECK(err, err = kernel_mscale.setArg(3, MAT_DIM1));

    printf("[OOO Queue]: Enqueueing scale kernel\n");
    OCL_CHECK(
        err,
        err = ooo_queue.enqueueNDRangeKernel(
            kernel_mscale, offset, global, local, nullptr, &ooo_events[0]));
    set_callback(ooo_events[0], "scale");

    // set OpenCL kernel parameters to add scaled matrix A and matrix B
    OCL_CHECK(err, err = kernel_madd.setArg(0, buffer_c));
    OCL_CHECK(err, err = kernel_madd.setArg(1, buffer_a));
    OCL_CHECK(err, err = kernel_madd.setArg(2, buffer_b));
    OCL_CHECK(err, err = kernel_madd.setArg(3, MAT_DIM0));
    OCL_CHECK(err, err = kernel_madd.setArg(4, MAT_DIM1));

    // This is an out of order queue, events can be executed in any order. Since
    // this call depends on the results of the previous call we must pass the
    // event object from the previous call to this kernel's event wait list.
    printf("[OOO Queue]: Enqueueing addition kernel (Depends on scale)\n");

    kernel_wait_events.resize(0);
    kernel_wait_events.push_back(ooo_events[0]);

    OCL_CHECK(err,
              err = ooo_queue.enqueueNDRangeKernel(
                  kernel_madd,
                  offset,
                  global,
                  local,
                  &kernel_wait_events, // Event from previous call
                  &ooo_events[1]));
    set_callback(ooo_events[1], "addition");

    // set OpenCL kernel parameters to multiply matrix D and E */
    OCL_CHECK(err, err = kernel_mmult.setArg(0, buffer_f));
    OCL_CHECK(err, err = kernel_mmult.setArg(1, buffer_d));
    OCL_CHECK(err, err = kernel_mmult.setArg(2, buffer_e));
    OCL_CHECK(err, err = kernel_mmult.setArg(3, MAT_DIM0));
    OCL_CHECK(err, err = kernel_mmult.setArg(4, MAT_DIM1));

    // This call does not depend on previous calls so we are passing nullptr
    // into the event wait list. The runtime should schedule this kernel in
    // parallel to the previous calls.
    printf("[OOO Queue]: Enqueueing matrix multiplication kernel\n");
    OCL_CHECK(err,
              err = ooo_queue.enqueueNDRangeKernel(
                  kernel_mmult,
                  offset,
                  global,
                  local,
                  nullptr, // Does not depend on previous call
                  &ooo_events[2]));
    set_callback(ooo_events[2], "matrix multiplication");

    const size_t array_size = MAT_DIM0 * MAT_DIM1;
    vector<int> A(array_size);
    vector<int> C(array_size);
    vector<int> F(array_size);

    // Depends on the addition kernel
    printf("[OOO Queue]: Enqueueing Read Buffer A (depends on addition)\n");
    kernel_wait_events.resize(0);
    kernel_wait_events.push_back(ooo_events[1]);
    OCL_CHECK(err,
              err = ooo_queue.enqueueReadBuffer(buffer_a,
                                                CL_FALSE,
                                                0,
                                                size_in_bytes,
                                                A.data(),
                                                &kernel_wait_events,
                                                &ooo_events[3]));
    set_callback(ooo_events[3], "A");

    printf("[OOO Queue]: Enqueueing Read Buffer C (depends on addition)\n");
    kernel_wait_events.resize(0);
    kernel_wait_events.push_back(ooo_events[1]);
    OCL_CHECK(err,
              err = ooo_queue.enqueueReadBuffer(buffer_c,
                                                CL_FALSE,
                                                0,
                                                size_in_bytes,
                                                C.data(),
                                                &kernel_wait_events,
                                                &ooo_events[4]));
    set_callback(ooo_events[4], "C");

    // Depends on the matrix multiplication kernel
    printf("[OOO Queue]: Enqueueing Read Buffer F (depends on matrix "
           "multiplication)\n");
    kernel_wait_events.resize(0);
    kernel_wait_events.push_back(ooo_events[2]);
    OCL_CHECK(err,
              err = ooo_queue.enqueueReadBuffer(buffer_f,
                                                CL_FALSE,
                                                0,
                                                size_in_bytes,
                                                F.data(),
                                                &kernel_wait_events,
                                                &ooo_events[5]));
    set_callback(ooo_events[5], "F");

    // Block until all operations have completed
    ooo_queue.flush();
    ooo_queue.finish();
    verify_results(C, F);
}

int main(int argc, char **argv) {

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];

    cl_int err;
    const size_t array_size = MAT_DIM0 * MAT_DIM1;
    const size_t size_in_bytes = array_size * sizeof(int);

    // allocate memory on host for input and output matrices
    vector<int, aligned_allocator<int>> A(array_size, 1);
    vector<int, aligned_allocator<int>> B(array_size, 1);
    vector<int, aligned_allocator<int>> D(array_size, 1);
    vector<int, aligned_allocator<int>> E(array_size, 1);

    // Called to set environment variables
    // The get_xil_devices will return vector of Xilinx Devices
    // platforms and will return list of devices connected to Xilinx platform
    auto devices = xcl::get_xil_devices();
    auto device = devices[0];

    OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(err,
              std::string device_name = device.getInfo<CL_DEVICE_NAME>(&err));

    // read_binary_file() is a utility API which will load the binaryFile
    // and will return pointer to file buffer.
   auto fileBuf = xcl::read_binary_file(binaryFile);
   cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    devices.resize(1);
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));

    OCL_CHECK(err, cl::Kernel kernel_madd(program, "madd", &err));
    OCL_CHECK(err, cl::Kernel kernel_mscale(program, "mscale", &err));
    OCL_CHECK(err, cl::Kernel kernel_mmult(program, "mmult", &err));

    // Allocate Buffer in Global Memory
    // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and
    // Device-to-host communication
    OCL_CHECK(err,
              cl::Buffer buffer_a(context,
                                  CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                                  size_in_bytes,
                                  A.data(),
                                  &err));
    OCL_CHECK(err,
              cl::Buffer buffer_b(context,
                                  CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                  size_in_bytes,
                                  B.data(),
                                  &err));
    OCL_CHECK(err,
              cl::Buffer buffer_c(context,
                                  CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY,
                                  size_in_bytes,
                                  NULL,
                                  &err));
    OCL_CHECK(err,
              cl::Buffer buffer_d(context,
                                  CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                  size_in_bytes,
                                  D.data(),
                                  &err));
    OCL_CHECK(err,
              cl::Buffer buffer_e(context,
                                  CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                  size_in_bytes,
                                  E.data(),
                                  &err));
    OCL_CHECK(err,
              cl::Buffer buffer_f(context,
                                  CL_MEM_ALLOC_HOST_PTR | CL_MEM_WRITE_ONLY,
                                  size_in_bytes,
                                  NULL,
                                  &err));

    // Use multiple command queues to execute the kernels
    multiple_command_queues(context,
                            device,
                            kernel_mscale,
                            kernel_madd,
                            kernel_mmult,
                            buffer_a,
                            buffer_b,
                            buffer_c,
                            buffer_d,
                            buffer_e,
                            buffer_f,
                            size_in_bytes);

    // Use out of order command queue to execute the kernels
    out_of_order_queue(context,
                       device,
                       kernel_mscale,
                       kernel_madd,
                       kernel_mmult,
                       buffer_a,
                       buffer_b,
                       buffer_c,
                       buffer_d,
                       buffer_e,
                       buffer_f,
                       size_in_bytes);


    printf("View the timeline trace in SDx for a visual overview of the\n"
           "execution of this example. Refer to the \"Timeline Trace\" section "
           "of\n"
           "the SDx Development Environment Methodology Guide for additional\n"
           "details.\n");

    printf("TEST PASSED\n");
    return EXIT_SUCCESS;
}
