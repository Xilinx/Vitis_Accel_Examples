Streaming Host to Kernel Bandwidth (C)
======================================

This is a simple Vector Increment C Kernel design with 1 Stream input and 1 Stream output that demonstrates on how to process an input stream of data for computation in an application and the Host to Device streaming bandwidth test.

**KEY CONCEPTS:** Read/Write Stream, Create/Release Stream

**KEYWORDS:** cl_stream, CL_STREAM_EOT, CL_STREAM_NONBLOCKING

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

zc
vck
xdma
qep
aws
samsung
::


DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/krnl_stream_adder1.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./vadd_stream <krnl_stream_adder1 XCLBIN>

COMMANDS FOR WINDOWS FLOW
-------------------------

Once the environment has been configured, run the following commands :

::

   cd cmake_build
   cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Debug -DXILINX_XRT=<set xilinx xrt path> -DOCL_ROOT=<set ocl root path>
   cmake --build . --verbose --config Debug --target install

   For Example : 
   cd cmake_build
   cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Debug -DXILINX_XRT=C:\Xilinx\XRT -DOCL_ROOT=C:\Xilinx\XRT\ext
   cmake --build . --verbose --config Debug --target install
