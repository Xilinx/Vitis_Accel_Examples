Xilinx XF_BLAS GEMM Example
===========================

Xilinx XF_BLAS GEMM Implementation for FP32 data type

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   _u2_
   u25
   u30
   u50
   zc
   vck
   nodma
   u250

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   include/hw/xf_blas/helpers/dataMover/bandedMatMoverB2.hpp
   include/hw/xf_blas/helpers/dataMover/gemmMatMover.hpp
   include/hw/xf_blas/helpers/dataMover/matMoverB2.hpp
   include/hw/xf_blas/helpers/dataMover/symMatMoverB2.hpp
   include/hw/xf_blas/helpers/dataMover/transpMatB2.hpp
   include/hw/xf_blas/helpers/dataMover/trmMatMoverB2.hpp
   include/hw/xf_blas/helpers/dataMover/vecMoverB1.hpp
   src/hw/kernel.cpp
   src/sw/api_gemm.cpp
   src/sw/xcl2/xcl2.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./api_gemm.exe <blas XCLBIN> 64 64 64 64 64 64 64 1 0

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.