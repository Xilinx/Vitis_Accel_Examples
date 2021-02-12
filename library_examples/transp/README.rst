Xilinx XF_BLAS Matrix Transposer
================================

A dense matrix transposer test

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   zc
   vck
   nodma

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/hw/kernel.cpp
   src/sw/transp_main.cpp
   src/sw/xcl2/xcl2.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./host.exe <transp XCLBIN> transp 256 512 A0 B0

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.