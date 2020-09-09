Stream Memory Map Mixed
=======================

This is a simple streaming Vector Addition C Kernel design with 1 Stream input, 1 memory mapped input to the kernel, and 1 stream output that demonstrates on how to process a stream of data along with memory mapped interface.

**KEY CONCEPTS:** Read/Write Stream, Create/Release Stream

**KEYWORDS:** cl_stream, CL_STREAM_EOT

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

   ./streaming_mm_mixed <krnl_stream_vadd XCLBIN>

