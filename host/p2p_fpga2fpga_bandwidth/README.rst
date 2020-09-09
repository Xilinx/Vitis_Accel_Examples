P2P FPGA to FPGA Bandwidth Example
==================================

This is simple example to explain performance bandwidth for P2P transfer between two FPGA devices.

**KEY CONCEPTS:** P2P, Multi-FPGA Execution, XDMA

**KEYWORDS:** XCL_MEM_EXT_P2P_BUFFER

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   zc
   201910
   2018
   vck
   aws
   u50

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/bandwidth.cpp
   src/host.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./host <bandwidth XCLBIN>

