Device Only Buffer XRT (XRT Native API's)
=========================================

This example will demonstrate how to create buffers in global memory which are not mapped to host.

**KEY CONCEPTS:** Device only buffer

**KEYWORDS:** xrt::bo::flags::device_only

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   nodma

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

   ./device_only_buffer -x <device_only_buf XCLBIN>

DETAILS
-------

This example illustrates the use of ``device_only`` Buffer which is
meant to be used only by the kernel.In this example we create buffer
in global memory which are not mapped to host.

.. code:: cpp

   xrt::bo::flags flags = xrt::bo::flags::device_only;
   auto bo_dev_only = xrt::bo(device, size_in_bytes, flags, krnl_mmult.group_id(0));

Using device_only flag we mention that the buffer is device only buffer
and the flag is passed as argument during buffer creation.


For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.