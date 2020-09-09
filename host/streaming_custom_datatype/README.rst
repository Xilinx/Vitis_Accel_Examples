Streaming Custom Data Type
==========================

This is simple streaming example of RGB to HSV conversion to demonstrate how streams of custom DATA Types needs to be handled on the kernel side.

**KEY CONCEPTS:** Custom Datatype

**KEYWORDS:** struct, #pragma HLS LOOP_TRIPCOUNT, cl_stream, CL_STREAM_EOT

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

   data/input.bmp
   src/host.cpp
   src/rgb_to_hsv.cpp
   src/rgb_to_hsv.h
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./streaming_custom_datatype -x <rgb_to_hsv XCLBIN> -i ./data/input.bmp

