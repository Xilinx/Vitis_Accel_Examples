Data Transfer
==============

This example describe various ways of data transfer between host and device.
Data is written and read back from the kernels using different APIs and  verified using `verify` function.


__Various APIs used are listed below :__

`CL_MEM_COPY_HOST_PTR` flag indicates OpenCL Runtime to use the host allocated pointers directly for buffer instead of allocating a new memory.

Following code-snippet shows how to create such buffer:
```c++
cl::Buffer buffer_in(context, CL_MEM_COPY_HOST_PTR, size_in_bytes, host_memory.data());
```
This kind of buffer help user to avoid two level of copy operations (host memory to buffer memory and later from buffer memory to Device memory). User can directly read/write host pointer and same pointer will be used by Xilinx Runtime to transfer the data to/from Device.

Another buffer `buffer` is created using `cl::buffer` with `CL_MEM_READ_ONLY` flag which indicates that Kernel can only read from this buffer.
```c++
 OCL_CHECK(err, cl::Buffer buffer(context, CL_MEM_READ_ONLY, size_in_bytes, nullptr, &err));
```

`clEnqueueWriteBuffer()` API is used to enqueue commands to write to a buffer object from host memory. 

```c++
OCL_CHECK(err, err = q.enqueueWriteBuffer(buffer,              // buffer on the FPGA
                          CL_FALSE,             // blocking call
                          0,                   // buffer offset in bytes
                          size_in_bytes,       // Size in bytes
                          host_memory2.data(),  // Pointer to the data to copy
                          nullptr, &async_event));
```

This operation moves data from host memory to buffer memory, but it does not transfer data from buffer memory to device memory. Actual data transfer to device happens, only when a Kernel is about to enqueue and one of the kernel argument is buffer. 

`clEnqueueReadBuffer()` API is used to enqueue commands to read the data from device memory to buffer object and once it is over, it copy data from buffer object to Host memory ptr.

```c++
err = q.enqueueReadBuffer(buffer,  // This buffers data will be read
                          CL_TRUE, // blocking call
                          0,       // offset
                          size_in_bytes,
                          host_memory.data(), // Data will be stored here
                          nullptr, &read_event);
```

`clEnqueueMapBuffer` enqueues a command to map a region of the buffer object into the host address space and `returns a pointer to this mapped region`. 
```c++
ptr = q.enqueueMapBuffer(buffer2, CL_TRUE, CL_MAP_READ,
                                0, size_in_bytes, nullptr, nullptr, &err);
```
Pointer returned can be used directly to modify the buffer object data. Flags `CL_MAP_READ` and `CL_MAP_WRITE` are used to specify read and write permissions for the kernel. 


`clEnqueueMigrateMemObjects` is another API used to transfer data between buffer memory and Device memory. It does immediate migration of data to DDR memory when it is called rather than when the kernel is enqueued for execution.
```c++
err = q.enqueueMigrateMemObjects({buffer_mem}, 0 /* 0 means from host*/ );
err = q.enqueueMigrateMemObjects({buffer_mem}, CL_MIGRATE_MEM_OBJECT_HOST);
```   

