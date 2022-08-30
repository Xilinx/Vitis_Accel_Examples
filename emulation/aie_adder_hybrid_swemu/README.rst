AIE Adder Hybrid SW Emulation using x86 PS 
===========================================

This example demonstrates how a user can run Hybrid SW Emulation as an x86 process instead of ARM process for an AIE design.

**KEY CONCEPTS:** `Compiling and Linking for x86 <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Compiling-and-Linking-for-x86>`__, `Compiling and Linking for Arm <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Compiling-and-Linking-for-Arm>`__, `AI Engine <https://docs.xilinx.com/r/en-US/ug1076-ai-engine-environment/AI-Engine/Programmable-Logic-Integration>`__, `Adaptive Data Flow <https://docs.xilinx.com/r/en-US/ug1076-ai-engine-environment/Adaptive-Data-Flow-Graph-Specification-Reference>`__, `Graph <https://docs.xilinx.com/r/en-US/ug1076-ai-engine-environment/graph>`__

**KEYWORDS:** `aiecompiler <https://docs.xilinx.com/r/en-US/ug1076-ai-engine-environment/Compiling-an-AI-Engine-Graph-Application>`__, `aiesimulator <https://docs.xilinx.com/r/en-US/ug1076-ai-engine-environment/Simulating-an-AI-Engine-Graph-Application>`__, `stream_connect <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Specifying-Streaming-Connections-between-Compute-Units>`__

.. raw:: html

 <details>

.. raw:: html

 <summary> 

 <b>EXCLUDED PLATFORMS:</b>

.. raw:: html

 </summary>
|
..

 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - AWS VU9P F1
 - Samsung SmartSSD Computation Storage Drive
 - Samsung U.2 SmartSSD
 - Versal VCK5000
 - All Xilinx DMA Platforms

.. raw:: html

 </details>

.. raw:: html

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   data/golden.txt
   data/input0.txt
   data/input1.txt
   src/aie/adder.cc
   src/aie/graph.cpp
   src/aie/graph.h
   src/aie/kernel.h
   src/pl_kernels/mm2s.cpp
   src/pl_kernels/s2mm.cpp
   src/sw/host.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./aie_adder_hybrid_swemu <krnl_adder XCLBIN>

DETAILS
-------

This example demonstrates how a user can run SW Emulation with cycle accurate AIE intended to provide cycle accuracy for AIE with fast, functional PL and PS running in SW emulation.  

The entire system design having AIE interfaced with PL kernels needs to be controlled through software using XRT APIs and such designs can be verified only through SW Emulation or HW Emulation flow.The cycle accuracy is very important aspect in AIE kernel development. SW_EMU does not provide cycle accuracy while HW_EMU does provides cycle accuracy but it is too slow for the users focused only on AIE. The hybrid flow intends to run SW Emulation with cycle accurate AIE keeping the PL as functional.

On the top, the PS on x86 flow in hybrid software emulation helps to run SW_EMU faster unlike the QEMU flow that requires overhead of creating sd_card.img, booting petalinux on full QEMU machine thereby taking additional time.  
 
Input from the User
-------------------

1. Compile the ADF graph for hw target 

        `aiecompiler --target=hw -Xpreproc=-D_HYBRID_EMU_ -include="$(XILINX_VITIS)/aietools/include" -include="./src/aie" -include="./data" -include="./" --pl-freq=100 -workdir=./_aie`

2. The vitis compile and link flow (v++ -c and v++ -l) must use the sw_emu target.  

3. The following environment variables on the terminal:
 
         `setenv XILINX_VITIS_EMU_HYBRID 1`: sets the Hybrid flow (required for link and runtime) 

4. Host compilation must be similar to ps_on_x86. 
 
          *Note:* This example Makefile already supports ps on x86 host compilation flow for hybrid software emulation. 

5. Use ``make run TARGET=sw_emu`` to run for the software emulation target. 

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.