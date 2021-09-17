Host Memory Test(C)
===================

This design is created to validate direct host memory access from kernel using host memory.

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

   sp=hostmemory_1.input:HOST[0]
   sp=hostmemory_1.output:HOST[0]

Following is the real log reported when running the design on U250:

:: 

   Found Platform
   Platform Name: Xilinx
   INFO: Reading ./test/hostmemory.xclbin
   Loading: './test/hostmemory.xclbin'
   Trying to program device xilinx_u250_gen3x16_xdma_3_202020_1
   Device program successful!
   Throughput: 15220 MB/s
   TEST PASSED
