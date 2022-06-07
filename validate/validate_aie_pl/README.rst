Xilinx AIE Validate Test
========================

Xilinx AIE Validate example

.. raw:: html

 <details>

.. raw:: html

 <summary> 

 <b>EXCLUDED PLATFORMS:</b>

.. raw:: html

 </summary>
|
..

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

.. raw:: html

 </details>

.. raw:: html

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