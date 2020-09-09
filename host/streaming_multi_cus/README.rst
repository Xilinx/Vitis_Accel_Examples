Stream Multiple Compute Units
=============================

This is a simple vadd Kernel design in which multiple compute units of kernel are directly communicating with host using stream interfaces.

**KEY CONCEPTS:** Read/Write Stream, Create/Release Stream

**KEYWORDS:** cl_stream, CL_STREAM_EOT, nk

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
   src/krnl_stream_vadd.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./streaming_multi_cus <krnl_stream_vadd XCLBIN>

