Hello World (HLS C/C++ Kernel)
==============================

This is simple example of vector addition to describe how to use HLS kernels in Vitis Environment.

**KEY CONCEPTS:** HLS C Kernel, OpenCL Host APIs

**KEYWORDS:** gmem, bundle, #pragma HLS INTERFACE, m_axi, s_axilite

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/vadd.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./hello_world <vadd XCLBIN>

