Global Memory Two Banks (OpenCL Kernel)
=======================================

This example of 2ddr to demonstrate on how to use 2ddr XSA. How to create buffers in each DDR.

**KEY CONCEPTS:** Multiple Banks

**KEYWORDS:** sp, connectivity

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   u50
   vck
   zc
   samsung

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   data/golden.bmp
   data/input.bmp
   src/apply_watermark.cl
   src/host.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./host -x <apply_watermark XCLBIN> -i ./data/input.bmp -c ./data/golden.bmp

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
