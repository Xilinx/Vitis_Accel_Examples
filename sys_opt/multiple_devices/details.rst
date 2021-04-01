Multiple Devices
================

This example demonstrates how multiple FPGA devices can be configured on
a system.

OpenCL context and queues are initialized for each FPGA.

.. code:: cpp

   contexts[d] =cl::Context(devices[d], props, nullptr, nullptr, &err);
   queues[d] = cl::CommandQueue(contexts[d], devices[d], CL_QUEUE_PROFILING_ENABLE, &err);

Each FPGA device needs to be loaded and programmed with binary file and
a kernel each is created for FPGAs on the system.

.. code:: cpp

   fileBuf[d] = xcl::read_binary_file(binaryFile, fileBufSize);
   bins[d].push_back({fileBuf[d], fileBufSize});
   programs[d] = load_cl2_binary(bins[d], devices[d], contexts[d]);
   kernels[d] = cl::Kernel(programs[d], "vadd", &err);

Buffers are also created for each FPGA seperately.

.. code:: cpp

   buffer_a[d] = cl::Buffer(contexts[d], CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY, size_per_device, &A[offset], &err);

Following table summarizes the observations while running the design on 1 and 2 U50 platforms:

============ =============
Device Count Time Taken(s) 
============ ============= 
1            47.41
2            23.71
Speedup      1.99
============ =============
