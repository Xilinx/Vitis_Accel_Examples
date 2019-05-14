Data Transfer
==============

This example deals with OpeCL APIs for data transfer between host and kernel.
Data is written and read back from the kernels using different APIs and  verified using `verify` function.

Host writes the data to be used by kernels during execution to the global memory using buffers. These buffers are accessible to kernels and they can read and write to these according to the permissions provided by the host in the API. Data written to these buffers by the kernels is then read back by the host.

__Various APIs used are listed below :__

`cl::Buffer` wraps `clCreateBuffer` API is used to create a buffer `buffer_in` in the global memory in a specified context. `CL_MEM_COPY_HOST_PTR` flag indicates that application wants the OpeCL implementation to allocate memory for the memory object and copy the data referenced by *hostptr.

```c++
OCL_CHECK(err, cl::Buffer buffer_in(context, CL_MEM_COPY_HOST_PTR, size_in_bytes, host_memory.data(), &err));
```

Another buffer `buffer` is created using `cl::buffer` with `CL_MEM_READ_ONLY` flag which indicates that FPGA can only read from this buffer.
```c++
 OCL_CHECK(err, cl::Buffer buffer(context, CL_MEM_READ_ONLY, size_in_bytes, nullptr, &err));
```

`clEnqueueWriteBuffer()` API is used to enqueue commands to write to a buffer object from host memory. This API writes the data to the buffer when the relevant kernel is enqueued for execution at runtime. Therefore, it is the responsibility of the user to ensure that data doesn't change between write call for bufer and the execution call for the kernel. `blocking_write` argument takes `CL_TRUE` and `CL_FALSE` flags which indicate whether the write is blocking or non-blocking.
```c++
OCL_CHECK(err, err = q.enqueueWriteBuffer(buffer,              // buffer on the FPGA
                          CL_FALSE,             // blocking call
                          0,                   // buffer offset in bytes
                          size_in_bytes,       // Size in bytes
                          host_memory2.data(),  // Pointer to the data to copy
                          nullptr, &async_event));
```
In case of non-blocking writes, `cl::event` object specified as the last argument of these APIs can be used to wait for the write operation to complete.
```c
 async_event.wait();
```
`clEnqueueReadBuffer()` API is used to enqueue
commands to read from a buffer object to host memory.Again, the call can be blocking or non-blocking as indicated by the flags.
```c++
OCL_CHECK(err, err = q.enqueueReadBuffer(buffer,  // This buffers data will be read
                          CL_TRUE, // blocking call
                          0,       // offset
                          size_in_bytes,
                          host_memory.data(), // Data will be stored here
                          nullptr, &read_event));
```
`clEnqueueMapBuffer` enqueues a command to map a region of the buffer object into the host address space and `returns a pointer to this mapped region`. This is another way to transfer data between host and kernel. Pointer returned can be used to modify the mapped data. Flags `CL_MAP_READ` and `CL_MAP_WRITE` are used to specify read and write permissions for the kernel. Mapping  can again be blocking or non blocking as specified by flags.
```c++
ptr = q.enqueueMapBuffer(buffer2, CL_TRUE, CL_MAP_READ,
                                0, size_in_bytes, nullptr, nullptr, &err);
```

*Notes :*

* If the map operation is CL_FALSE(non-blocking), the pointer returned by the API can't be used until the map has completed.

* The behavior of OpenCL function calls that enqueue commands that write or copy to regions of a memory object that are mapped is undefined.

`clEnqueueMigrateMemObjects` is another API used to transfer data between host and the kernels. It does immediate migration of data to DDR memory when it is called rather than when the kernel is enqueued for execution.
```c++
OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_mem}, 0 /* 0 means from host*/ ));
   OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_mem}, CL_MIGRATE_MEM_OBJECT_HOST));
```   

