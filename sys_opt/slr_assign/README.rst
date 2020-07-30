SLR Assign (CL)
===============

This is simple example to describe SLR assignment information for a platform design. This example highlights how to provide extra input to assign the logic of the kernel into a nominated SLR. In this example we are assigning first kernel(Vector Multiplication) to SLR0 and assigning the second kernel(Vector Addition) to SLR1

**KEY CONCEPTS:** SLR Assignments

**KEYWORDS:** slr

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   _u25_
   zc
   vck
   samsung

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/kernel.cl
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./host <vmult_vadd XCLBIN>

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
