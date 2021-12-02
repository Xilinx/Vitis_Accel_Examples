Custom Data Type (C)
====================

This is simple example of RGB to HSV conversion to demonstrate Custom DATA Type usages in C Based Kernel. Xilinx HLS Compiler Supports Custom Data Type to use for operation as well as Memory Interface between Kernel and Global Memory.

**KEY CONCEPTS:** `Custom Datatype <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Composite-Data-Types>`__

**KEYWORDS:** `struct <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Structs>`__, `#pragma HLS LOOP_TRIPCOUNT <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-loop_tripcount>`__

**EXCLUDED PLATFORMS:** 

 - All NoDMA Platforms, i.e u50 nodma etc

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/rgb_to_hsv.cpp
   src/rgb_to_hsv.h
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./custom_datatype -x <rgb_to_hsv XCLBIN> -i $(XF_PROJ_ROOT)/common/data/xilinx_logo.bmp

DETAILS
-------

Kernel ports can have custom datatypes.It is recommended that custom
datatype is a power of 2 and minimum 32 bits to allow ``burst transfer``
thus using the AXI master bandwidth efficiently. Extra ``padding`` can
be added in case not a multiple of 32 bits as shown below.

.. code:: cpp

   typedef struct RGBcolor_struct
   {
     unsigned char r;
     unsigned char g;
     unsigned char b;
     unsigned char pad;
   } RGBcolor;

   typedef struct HSVcolor_struct
   {
     unsigned char h;
     unsigned char s;
     unsigned char v;
     unsigned char pad;
   }HSVcolor;

Kernel in this example uses the above structures as datatypes for its
input and output ports.

::

   void rgb_to_hsv(RGBcolor *in,  // Access global memory as RGBcolor struct-wise
                   HSVcolor *out, // Access Global Memory as HSVcolor struct-wise
                   int size) 

Custom datatypes can be used to reduce the number of
``kernel arguments`` thus reducing the number of interfaces between
kernels and memory. It can also help to reduce execution time to set
kernel arguments if number of kernel arguments is large.

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.