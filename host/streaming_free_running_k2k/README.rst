Stream Free Running Kernel (HLS C/C++)
======================================

This is simple example which demonstrate how to use and configure a free running kernel.

**KEY CONCEPTS:** Free Running Kernel

**KEYWORDS:** ap_ctrl_none, stream_connect

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   xdma
   qep
   aws
   samsung
   zc702

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/increment.cpp
   src/mem_read.cpp
   src/mem_write.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./streaming_free_running_k2k <krnl_incr XCLBIN>

