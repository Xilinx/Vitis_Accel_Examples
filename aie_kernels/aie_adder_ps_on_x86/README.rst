AIE Adder Embedded SW-Emulation using x86 PS 
=============================================

This example demonstrates how a user can run the PS App as an x86 process instead of ARM process for an AIE design.

**KEY CONCEPTS:** `Compiling and Linking for x86 <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Compiling-and-Linking-for-x86>`__, `Compiling and Linking for Arm <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Compiling-and-Linking-for-Arm>`__, `AI Engine <https://docs.xilinx.com/r/en-US/ug1076-ai-engine-environment/AI-Engine/Programmable-Logic-Integration>`__, `Adaptive Data Flow <https://docs.xilinx.com/r/en-US/ug1076-ai-engine-environment/Adaptive-Data-Flow-Graph-Specification-Reference>`__, `Graph <https://docs.xilinx.com/r/en-US/ug1076-ai-engine-environment/graph>`__

**KEYWORDS:** `aiecompiler <https://docs.xilinx.com/r/en-US/ug1076-ai-engine-environment/Compiling-an-AI-Engine-Graph-Application>`__, `aiesimulator <https://docs.xilinx.com/r/en-US/ug1076-ai-engine-environment/Simulating-an-AI-Engine-Graph-Application>`__, `stream_connect <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Specifying-Streaming-Connections-between-Compute-Units>`__

**EXCLUDED PLATFORMS:** 

 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - AWS VU9P F1
 - Samsung SmartSSD Computation Storage Drive
 - Samsung U.2 SmartSSD
 - Versal VCK5000
 - All Xilinx DMA Platforms

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   data/golden.txt
   data/input0.txt
   data/input1.txt
   src/aie_adder.cc
   src/aie_graph.cpp
   src/aie_graph.h
   src/aie_kernel.h
   src/host.cpp
   src/pl_mm2s.cpp
   src/pl_s2mm.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./aie_adder_ps_on_x86 <krnl_adder XCLBIN>

DETAILS
-------

This example demonstrates how SW Emulation for Embedded platforms can run the PS App as an x86 process instead of ARM process for an AIE Design. SW Emulation is an abstract model and does not use any of the petalinux drivers like ZOCL, interrupt controller or DTB. Hence, the overhead of creating sd_card.img, booting petalinux on full QEMU machine is too heavy weight and should be avoided for SW Emulation.

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.