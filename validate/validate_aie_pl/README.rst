Xilinx Accumulate AIE Codec Test
================================

Accumulate AIE function example

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   data/input1.txt
   src/aie/forward.cpp
   src/aie/forward.hpp
   src/aie/graph.cpp
   src/aie/graph.h
   src/pl/mm2s.cpp
   src/pl/s2mm.cpp
   src/ps/host.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./host.elf

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.