Host Memory Test(C)
===================

This host memory design explains how direct host memory access can be done by the kernel.

**KEY CONCEPTS:** host memory, address translation unit

**KEYWORDS:** XCL_MEM_EXT_HOST_ONLY, HOST[0]

**EXCLUDED PLATFORMS:** 

 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - All Versal Platforms, i.e vck190 etc
 - AWS VU9P F1
 - All Platforms with 2019 Version
 - All Platforms with 2018 Version
 - Samsung U.2 SmartSSD
 - Alveo U25 SmartNIC
 - Alveo U30
 - Alveo U50lv
 - Alveo U50 gen3x4

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/hostmemory.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./hostmemory.exe -p $(GEN_DIR)

DETAILS
-------

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

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.