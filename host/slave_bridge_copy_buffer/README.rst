Slave Bridge Copy Buffer(C)
===========================

This is simple slave bridge example to describe how host-only memory can be copied to device-only memory and vice-versa.

**KEY CONCEPTS:** slave bridge

**KEYWORDS:** XCL_MEM_EXT_HOST_ONLY, CL_MEM_HOST_NO_ACCESS, enqueueCopyBuffer

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   _u25_
   u30
   zc
   vck
   aws-vu9p-f1
   2019
   2018
   202010
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

   ./slave_bridge_copy_buffer.exe <krnl_vadd XCLBIN>

