Slave Bridge Copy Buffer XRT (XRT Native API's)
===============================================

This is simple slave bridge example to describe how host-only memory can be copied to device-only memory and vice-versa using XRT Native API's.

**KEY CONCEPTS:** slave bridge

**KEYWORDS:** host_only, device_only, HOST[0]

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   _u25_
   u30
   u50c
   u50lv
   u50_gen3x4
   u55
   zc
   vck
   aws
   2019
   2018
   samsung
   u250
   _u2_

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/krnl_vadd.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./slave_bridge_copy_buffer_xrt -x <krnl_vadd XCLBIN>

DETAILS
-------

This example illustrates how host only memory can be copied to device only memory and vice-versa using native xrt ``copy`` API.
 
Host only buffer is created using ``host_only`` flag whereas device only buffer is created using ``device_only`` flag. The following data transfers take place in the example -

1. Copy the host_only input buffer ``hostonly_bo`` into  DEVICE_ONLY input buffer ``deviceonly_bo`` using M2M.

.. code:: cpp

  deviceonly_bo0.copy(hostonly_bo0);
  deviceonly_bo1.copy(hostonly_bo1);

2. Enqueue kernel to do computations on DEVICE_ONLY input buffer ``deviceonly_bo`` and put output in device_only output buffer ``deviceonly_bo_out``.

.. code:: cpp

  auto run = krnl(deviceonly_bo0, deviceonly_bo1, deviceonly_bo_out, DATA_SIZE);
  
3. Copy the device_only output buffer ``deviceonly_bo_out`` into  host_only output buffer ``hostonly_bo_out`` using M2M.

.. code:: cpp

  hostonly_bo_out.copy(deviceonly_bo_out);

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.