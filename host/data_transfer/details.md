Data Transfer
===========================

This example illustrates how to transfer data back and forth between host and device.

There are several ways to creating openCL buffers. The most recommended way is to create the cl::Buffer object using the CL_MEM_COPY_HOST_PTR flag as below:
```c++
std::vector<int, aligned_allocator<int>> host_memory(elements, 42);
cl::Buffer buffer_in(context, CL_MEM_COPY_HOST_PTR, size_in_bytes,host_memory.data(),nullptr));
```
For above case, it expect that Host memory pointer should be aligned to Memory for the same aligned_allocator is used. If host pointer is not aligned, it will generate the warning and will involve extra memcpy from host memory to Buffer Memory.

After creating buffer using Host Mem Pointer, clEnqueueMigrateMemObjects can be used for immediate migration of data without considering the fact that data is actually needed or not by kernel operation.
```c++
cl::Buffer buffer_mem(context, CL_MEM_USE_HOST_PTR,size_in_bytes, host_memory2.data(),&err));
              q.enqueueMigrateMemObjects({buffer_mem},0 /* 0 means from host*/));
              q.enqueueMigrateMemObjects({buffer_mem},CL_MIGRATE_MEM_OBJECT_HOST));
```

While creating buffer, Buffer type can also be specified as below:
```c++
cl::Buffer buffer_in(context, CL_MEM_READ_ONLY, size_in_bytes);
cl::Buffer buffer_out(context, CL_MEM_WRITE_ONLY, size_in_bytes);
cl::Buffer buffer_inout(context, CL_MEM_READ_WRITE, size_in_bytes);
```
The CL_MEM_READ_ONLY flag indicates that the Kernel can only read from this location. The host can write only to this buffer.
The CL_MEM_WRITE_ONLY flag indicates that the Kernel can only Write to this location. The host can read only from this buffer.
The CL_MEM_READ_WRITE flag indicates that the both kernel and host can do read and write operation to this location.

To transfer the data from host Buffer memory to device memory enqueueWriteBuffer API can be used as below:
```c++
q.enqueueWriteBuffer(
  buffer,             // buffer on the FPGA
  CL_FALSE,            // non-blocking call
  0,                  // buffer offset in bytes
  size_in_bytes,      // Size in bytes
  host_memory.data(), // Pointer to the data to copy
  );
```
`enqueueWriteBuffer()` API call is a request to enqueue write operation. This API call does not immediately initiate the data transfer. Data transfer happens when a kernel is enqueue which has respective buffer as one of the kernel arguments. So for such kernel call, runtime first transfer the data from host to device and later trigger the kernel call.


Data can be transferred back to the host using the read buffer operation as below:

```c++
q.enqueueReadBuffer(
    buffer,  // This buffers data will be read
    CL_TRUE, // blocking call
    0,       // offset
    size_in_bytes,
    host_memory.data(), // Data will be stored here
);
```

Mapping and unmapping buffers is another way to transfer memory to and from the FPGA. This operation gives you a pointer that can be freely modified by your host application

```c++
void *ptr = q.enqueueMapBuffer(
    buffer,       // buffer
    CL_TRUE,      // blocking call
    CL_MAP_WRITE, // Indicates we will be writing
    0,            // buffer offset
    size_in_bytes // size in bytes
); 
int *data_ptr = reinterpret_cast<int *>(ptr);
```
You can now assign values to the pointer  `data_ptr` just like a regular pointer. The buffer must be unmapped before it can be used in other operations

```c++
q.enqueueUnmapMemObject(buffer, ptr /*pointer returned by Map call*/);
```

