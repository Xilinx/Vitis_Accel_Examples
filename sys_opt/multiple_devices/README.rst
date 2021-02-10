Multiple Devices (C)
====================

This example show how to take advantage of multiple FPGAs on a system. It will show how to initialized an OpenCL context, allocate memory on the two devices and execute a kernel on each FPGA.

**KEY CONCEPTS:** OpenCL API, Multi-FPGA Execution, Event Handling

**KEYWORDS:** xcl::get_xil_devices

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   nodma

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/vector_addition.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./multiple_devices <vector_addition XCLBIN>

DETAILS
-------

This example demonstrates how multiple FPGA devices can be configured on
a system.

OpenCL context and queues are initialized for each FPGA.

.. code:: cpp

   contexts[d] =cl::Context(devices[d], props, NULL, NULL, &err);
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

Kernels are enqueued into queues for each FPGA and ``cl_events`` can be
used to synchronize the kernel operations across devices.

.. code:: cpp

   err = queues[d].enqueueTask(kernels[d], NULL, &events[d]);

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.