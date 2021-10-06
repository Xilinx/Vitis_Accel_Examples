Verify Kernel (C)
=================

This is a simple design to verify that the platform has basic functionality.

**EXCLUDED PLATFORMS:** 

 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - Versal VCK190
 - All Platforms with 2019 Version
 - All Platforms with 2018 Version

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/verify.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./validate.exe -p $(GEN_DIR)

DETAILS
-------

This is an extremely simple HLS C Kernel and accompanying host code to verify that the platform has basic functionality. It will make a call to the kernel with an empty global buffer. The kernel will then write the string of characters "Hello World\n\0" into the buffer and return. The host will copy this buffer locally and then print out the resulting buffer contents.

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.