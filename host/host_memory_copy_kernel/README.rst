Host Memory Copy Kernel(C)
==========================

This is a Host Memory Example to describe how data can be copied between host-only buffer and device-only buffer using User Copy Kernel.

**KEY CONCEPTS:** host memory

**KEYWORDS:** XCL_MEM_EXT_HOST_ONLY, CL_MEM_HOST_NO_ACCESS, `enqueueMapBuffer <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Buffer-Creation-and-Data-Transfer>`__

**EXCLUDED PLATFORMS:** 

 - Alveo U25 SmartNIC
 - Alveo U30
 - Alveo U50lv
 - Alveo U50 gen3x4
 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - All Versal Platforms, i.e vck190 etc
 - All Platforms with 2019 Version
 - All Platforms with 2018 Version
 - Samsung U.2 SmartSSD

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

   ./host_memory_copy_kernel.exe <krnl_vadd XCLBIN>

DETAILS
-------

Here a user copy kernel is instantiated inside container which copies data between host and device memory.
 
Host only buffer is created using ``XCL_MEM_EXT_HOST_ONLY`` flag whereas device only buffer is created using ``CL_MEM_HOST_NO_ACCESS`` flag. The following data transfers take place in the example -

1. Enqueue kernel ``copy_kernel`` to copy the HOST_ONLY input buffer ``buffer_in_host`` into  DEVICE_ONLY input buffer ``buffer_in_dev``.

2. Enqueue kernel ``krnl_vadd`` to do computations on DEVICE_ONLY input buffer ``buffer_in_dev`` and put output in DEVICE_ONLY output buffer ``buffer_out_dev``.

3. Enqueue kernel ``copy_kernel`` to copy the DEVICE_ONLY output buffer ``buffer_out_dev`` into  HOST_ONLY output buffer ``buffer_out_host``.

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.