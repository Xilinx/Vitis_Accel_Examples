Overlap Host and HLS kernels (C)
=================================

This example illustrates how host's read and write tasks can be synchronized with kernel's computational tasks to achieve low latency in cases where data is too large to fit on FPGA or it is being streamed from a sensor or a network.

PCIe is a high bandwidth full duplex interconnect between host and FPGA. It can read and write data to FPGA simultaneously with low latency. This allows overlap of host and kernel tasks.


 `Kernel vadd` is a simple addition kernel which adds two numbers from buffers a and b  and stores the result in buffer c. Each of a,b and c have `two buffers` which are enqueued for write,read operations alternatively using a flag so that the hardware can be kept busy by the previous iteration while next iteration's data is being written into the buffer.
```c++
cl::Buffer buffer_a[2], buffer_b[2], buffer_c[2];
std::cout << "Creating Buffers..." << std::endl;
  OCL_CHECK(err, buffer_a[flag] = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
          bytes_per_iteration, &A[iteration_idx * elements_per_iteration], &err));
  OCL_CHECK(err, buffer_b[flag] = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
          bytes_per_iteration, &B[iteration_idx * elements_per_iteration], &err));
  OCL_CHECK(err, buffer_c[flag] = cl::Buffer(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
          bytes_per_iteration, &device_result[iteration_idx * elements_per_iteration], &err));
```



 Results computed by the kernel in the present cycle can be read by the host up until the next cycle finishes. This allows the operation to go on in a pipelined manner.

 OpenCL read and writes can be made asynchronous by using `CL_FALSE` flag in APIs and `cl::Event` objects can be used to synchronize these operations. Such as computation on data (kernel enqueue)  should not start until the data has been written into the relevant buffers.
```c++
OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_a[flag], buffer_b[flag]}, 0/*0 means from host*/, NULL, &write_event[0]));
  set_callback(write_event[0], "ooo_queue");
  std::vector<cl::Event> waitList;
    waitList.push_back(write_event[0]);
    OCL_CHECK(err, err = q.enqueueNDRangeKernel(krnl_vadd, 0, 1, 1, &waitList, &kernel_events[flag]));
```

