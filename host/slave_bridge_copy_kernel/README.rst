Slave Bridge Copy Kernel(C)
===========================

This is a Slave Bridge Example to describe how data can be copied between host-only buffer and device-only buffer using User Copy Kernel.

**KEY CONCEPTS:** slave bridge

**KEYWORDS:** XCL_MEM_EXT_HOST_ONLY, CL_MEM_HOST_NO_ACCESS, `enqueueMapBuffer <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/opencl_programming.html#czb1555520653128>`__

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   _u25_
   u30
   u50lv
   u50_gen3x4
   zc
   vck
   aws-vu9p-f1
   2019
   2018
   _u2_

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/copy_kernel.cpp
   src/host.cpp
   src/krnl_vadd.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./slave_bridge_copy_kernel.exe <krnl_vadd XCLBIN>

DETAILS
-------

Here a user copy kernel is instantiated inside container which copies data between host and device memory.
 
Host only buffer is created using ``XCL_MEM_EXT_HOST_ONLY`` flag whereas device only buffer is created using ``CL_MEM_HOST_NO_ACCESS`` flag. The following data transfers take place in the example -

1. Enqueue kernel ``copy_kernel`` to copy the HOST_ONLY input buffer ``buffer_in_host`` into  DEVICE_ONLY input buffer ``buffer_in_dev``.

2. Enqueue kernel ``krnl_vadd`` to do computations on DEVICE_ONLY input buffer ``buffer_in_dev`` and put output in DEVICE_ONLY output buffer ``buffer_out_dev``.

3. Enqueue kernel ``copy_kernel`` to copy the DEVICE_ONLY output buffer ``buffer_out_dev`` into  HOST_ONLY output buffer ``buffer_out_host``.

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.