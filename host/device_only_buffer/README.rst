Device Only Buffer (C)
======================

This example will demonstrate how to create buffers in global memory which are not mapped to host.

**KEY CONCEPTS:** Device only buffer

**KEYWORDS:** CL_MEM_HOST_NO_ACCESS

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

