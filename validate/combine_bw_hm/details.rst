Combine Bandwidth & Hostmemory(C)
=================================

This example contains verify test, bandwidth test and host memory test kernels to validate FPGA.

In the verify test we have extremely simple HLS C Kernel to verify that the platform has basic functionality. It will make a call to the kernel with an empty global buffer. The kernel will then write the string of characters "Hello World" into the buffer and return. The host will copy this buffer locally and then print out the resulting buffer contents.

In the bandwidth test we try to get the maximum possible bandwidth between Kernel and Global Memory.
Extracting Memory Information and generate cfg file:
   
      - Platforminfo -j (path to xpfm) > platform_info.json
      - From the platform_info.json file we pick the memory info
      - Generate the meta data related to Memory Banks(DDR/HBM/HOST) to platform_bandwidth.cfg file


Using the ``sp`` option  in the platform_bandwidth.cfg file AXI-Master Port is connected to the IP. 

::

   sp=bandwidth_1.input:DDR[0]
   sp=bandwidth_1.output:DDR[0]

hostmemory test is to validate direct host memory access from kernel using slave bridge.

The host allocates a buffer into specific host-only buffer using ``XCL_MEM_EXT_HOST_ONLY``. The ``cl_mem_ext_ptr`` object needs to be used in cases where memory assignment is done by user explicitly:

.. code:: cpp

   cl_mem_ext_ptr_t input_buffer_ext;
   input_buffer_ext.flags = XCL_MEM_EXT_HOST_ONLY;
   input_buffer_ext.obj = nullptr;
   input_buffer_ext.param = 0;
   
   OCL_CHECK(err, input_buffer[i] = new cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, vector_size_bytes,
                                             &input_buffer_ext, &err));

Using the ``sp`` option  in the platform_hostmemory.cfg file, AXI-Master Port is connected to the Slave-Bridge IP:

::

   sp=hostmemory.input:HOST[0]
   sp=hostmemory.output:HOST[0]
