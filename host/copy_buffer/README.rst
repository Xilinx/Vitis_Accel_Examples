Copy Buffer (C)
===============

This Copy Buffer example demonstrate how one buffer can be copied from another buffer.

**KEY CONCEPTS:** Copy Buffer

**KEYWORDS:** cl::CommandQueue, enqueueCopyBuffer, enqueueWriteBuffer, enqueueReadBuffer, enqueueMigrateMemObjects

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   xilinx_u200_xdma_201830
   xilinx_u250_xdma_201830

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/vector_addition.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./copy_buffer <vector_addition XCLBIN>

