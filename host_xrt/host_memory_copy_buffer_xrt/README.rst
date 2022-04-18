Host Memory Copy Buffer XRT (XRT Native API's)
==============================================

This is simple host memory example to describe how host-only memory can be copied to device-only memory and vice-versa using XRT Native API's.

**KEY CONCEPTS:** host memory

**KEYWORDS:** host_only, device_only, HOST[0]

**EXCLUDED PLATFORMS:** 

 - Alveo U25 SmartNIC
 - Alveo U30
 - Alveo U50c
 - Alveo U50lv
 - Alveo U50 gen3x4
 - Alveo U55
 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - All Versal Platforms, i.e vck190 etc
 - All Platforms with 2019 Version
 - All Platforms with 2018 Version
 - Samsung SmartSSD Computation Storage Drive
 - Alveo U250
 - Samsung U.2 SmartSSD

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

   ./host_memory_copy_buffer_xrt -x <krnl_vadd XCLBIN>

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