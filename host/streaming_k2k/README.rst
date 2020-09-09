Stream Kernel to Kernel
=======================

This is a simple kernel to kernel streaming Vector Add and Vector Multiply C Kernel that demonstrates on how to process a stream of data for computation between two kernels.

**KEY CONCEPTS:** Read/Write Stream, Create/Release Stream

**KEYWORDS:** cl_stream, CL_STREAM_EOT, CL_STREAM_NONBLOCKING

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
   src/krnl_stream_vmult.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./streaming_k2k <krnl_stream_vadd_vmult XCLBIN>

