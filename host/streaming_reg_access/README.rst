Control and Status Register access continuously streaming kernel (HLS C/C++)
============================================================================

This is simple example which demonstrate streaming free-running kernel with scalar input and output.

**KEY CONCEPTS:** Free Running Kernel

**KEYWORDS:** xclRegRead, xclRegWrite

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   u250
   zc
   vck
   xdma
   qep
   aws
   samsung

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/increment.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./streaming_reg_access -x <krnl_incr XCLBIN> -i 5

