PS Validate Kernel (C)
====================

This PS AIE Kernel test PS kernel controlling AI Engine graph.

**EXCLUDED PLATFORMS:** 

 - Alveo U25 SmartNIC
 - Alveo U30
 - Alveo U200
 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - Versal VCK190
 - Alveo U250
 - AWS VU9P F1
 - Samsung SmartSSD Computation Storage Drive
 - Samsung U.2 SmartSSD
 - X3 Compute Shell
 - All NoDMA Platforms, i.e u50 nodma etc

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/graph.cpp
   src/graph.h
   src/kernels/mm.cc
   src/kernels.h
   src/kernels.h
   src/sk_types.h
   src/include.h
   src/host.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./ps_aie.exe -p $(GEN_DIR)

DETAILS
-------

This example contains PS kernel with AIE graph test to validate AI engine from PS kernel.

Following is the real log reported when running the design on VCK5000:

:: 

   TEST PASSED

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
