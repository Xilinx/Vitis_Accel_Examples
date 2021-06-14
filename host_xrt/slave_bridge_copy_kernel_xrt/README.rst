Slave Bridge Copy Kernel XRT (XRT Native API's)
===============================================

This is a Slave Bridge Example to describe how data can be copied using xrt native api's between host-only buffer and device-only buffer using User Copy Kernel.

**KEY CONCEPTS:** slave bridge

**KEYWORDS:** host_only, device_only

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
   aws
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

   ./slave_bridge_copy_kernel_xrt -x <krnl_vadd XCLBIN>

DETAILS
-------

Here a user copy kernel is instantiated inside container which copies data between host and device memory.
 
Host only buffer is created using ``host_only`` flag whereas device only buffer is created using ``device_only`` flag. The following data transfers take place in the example -

1. Enqueue kernel ``copy_kernel`` to copy the HOST_ONLY input buffer ``hostonly_bo`` into  DEVICE_ONLY input buffer ``deviceonly_bo``.

2. Enqueue kernel ``krnl_vadd`` to do computations on DEVICE_ONLY input buffer ``deviceonly_bo`` and put output in DEVICE_ONLY output buffer ``deviceonly_bo_out``.

3. Enqueue kernel ``copy_kernel`` to copy the DEVICE_ONLY output buffer ``deviceonly_bo_out`` into  HOST_ONLY output buffer ``hostonly_bo_out``.

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.