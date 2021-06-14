Slave Bridge Simple XRT (XRT Native API's)
==========================================

This is simple slave bridge example to describe how a user kernel can access the host memory using xrt native api's.

**KEY CONCEPTS:** slave bridge, address translation unit

**KEYWORDS:** host_only, HOST[0]

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
   samsung
   _u2_

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/kernel.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./slave_bridge_simple_xrt -x <krnl_vadd XCLBIN>

DETAILS
-------

This example illustrates the use of ``host_only`` Buffer which resides
in the host memory and directly transferred to/from the kernel

.. code:: cpp

   xrt::bo::flags flags = xrt::bo::flags::host_only;
   auto bo0 = xrt::bo(device, size_in_bytes, flags, krnl.group_id(0));

Using host_only flag we mention that the buffer is host only buffer
and the flag is passed as argument during buffer creation.


For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.