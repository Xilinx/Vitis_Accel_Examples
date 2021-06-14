Slave Bridge (C)
================

This slave bridge bandwidth design explains how direct host memory access can be done by the kernel.

**KEY CONCEPTS:** slave bridge, address translation unit

**KEYWORDS:** XCL_MEM_EXT_HOST_ONLY, HOST[0]

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   zc
   vck
   aws-vu9p-f1
   2019
   2018
   _u2_
   _u25_
   u30
   u50lv
   u50_gen3x4

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/slavebridge.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./slavebridge.exe platform_test_path

DETAILS
-------

This design is created to validate direct host memory access from kernel using slave bridge.

The host allocates a buffer into specific host-only buffer using ``XCL_MEM_EXT_HOST_ONLY``. The ``cl_mem_ext_ptr`` object needs to be used in cases where memory assignment is done by user explicitly:

.. code:: cpp

   cl_mem_ext_ptr_t input_buffer_ext;
   input_buffer_ext.flags = XCL_MEM_EXT_HOST_ONLY;
   input_buffer_ext.obj = nullptr;
   input_buffer_ext.param = 0;
   
   OCL_CHECK(err, input_buffer[i] = new cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, vector_size_bytes,
                                             &input_buffer_ext, &err));

Using the ``sp`` option  in the platform_slavebridge.cfg file, AXI-Master Port is connected to the Slave-Bridge IP:

::

   sp=slavebridge_1.input:HOST[0]
   sp=slavebridge_1.output:HOST[0]

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.