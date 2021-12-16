Xilinx AIE Validate Test
========================

Xilinx AIE Validate example

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   pl_controller/L2/src/hw/pl_controller_kernel.cpp
   pl_controller/L2/src/sw/pl_controller.cpp
   pl_ctrl_aie_asyn/src/aie/graph.cpp
   pl_ctrl_aie_asyn/src/aie/graph.h
   pl_ctrl_aie_asyn/src/aie/kernels.h
   pl_ctrl_aie_asyn/src/aie/kernels/include.h
   pl_ctrl_aie_asyn/src/aie/kernels/kernels.cc
   pl_ctrl_aie_asyn/src/host/host.cpp
   pl_ctrl_aie_asyn/src/xclbin/pl_kernels/sender_receiver.cpp
   pl_ctrl_aie_asyn/src/xclbin/pl_kernels/sender_receiver.hpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./host.exe

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.