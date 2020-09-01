Xilinx MC European Engine Benchmark
===================================

Xilinx XF_FINTECH Test

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   zc

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/test.cpp
   src/utils.hpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./test.exe  -xclbin <kernel_mc XCLBIN>

