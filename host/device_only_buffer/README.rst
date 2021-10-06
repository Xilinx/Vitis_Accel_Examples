Device Only Buffer (C)
======================

This example will demonstrate how to create buffers in global memory which are not mapped to host. The device only memory allocation is done through the host code. The kernel can read data from device memory and write result to device memory.

**KEY CONCEPTS:** Device only buffer

**KEYWORDS:** CL_MEM_HOST_NO_ACCESS

**EXCLUDED PLATFORMS:** 

 - All NoDMA Platforms, i.e u50 nodma etc

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/madd.cpp
   src/mmult.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./device_only_buffer <device_only_buf XCLBIN>

DETAILS
-------

Device only buffers are used by the kernels to read and write data directly from/to the Device Memory. When xrt allocates memory on the device for a buffer object, it tries to determine which bank to allocate the buffer. Bank assignment can be done explicitly in the host application through the use of cl_mem_ext_ptr or can be automatically inferred based on device CU connectivity.

Host code Guidelines
--------------------------

XRT OpenCL introduces a new buffer extension Flag ``CL_MEM_HOST_NO_ACCESS`` that should be used to denote a Device-only buffer

.. code-block:: c++

    OCL_CHECK(err, cl::Buffer dev_only_buf(context, CL_MEM_READ_WRITE |
                                                      CL_MEM_HOST_NO_ACCESS,
                                         size_in_bytes, nullptr, &err));



For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.