Multiple Devices
====================

This example demonstrates how multiple FPGA devices can be configured on a system.

OpenCL context and queues are initialized for each FPGA.
```c++
contexts[d] =cl::Context(devices[d], props, NULL, NULL, &err);
queues[d] = cl::CommandQueue(contexts[d], devices[d], CL_QUEUE_PROFILING_ENABLE, &err);
```  

Each FPGA device needs to be loaded and programmed with binary file and a kernel each is created for FPGAs on the system.
```c++                     
fileBuf[d] = xcl::read_binary_file(binaryFile, fileBufSize);
bins[d].push_back({fileBuf[d], fileBufSize});
programs[d] = load_cl2_binary(bins[d], devices[d], contexts[d]);
kernels[d] = cl::Kernel(programs[d], "vadd", &err);
```
Buffers are also created for each FPGA seperately.
```c++
buffer_a[d] = cl::Buffer(contexts[d], CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, size_per_device, &A[offset], &err);
```
Kernels are enqueued into queues for each FPGA and `cl_events` can be used to synchronize the kernel operations across devices. 
```c++
err = queues[d].enqueueTask(kernels[d], NULL, &events[d]);
```

