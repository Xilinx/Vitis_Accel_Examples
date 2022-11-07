AIE App with PL as HLS functional kernels
=========================================

This example demonstrates the flow on how to connect aie Kernel to PL HLS functional Kernels and validate the design running on the AI Engine. The PL kernels in this AIE design are simulated in the functional mode. The functional mode in v++ generates the SCTLM code for the HLS kernel. The functional model of the PL HLS kernel is applicable when user wants to speedup the emulation by compiling desired kernels in functional mode instead of the default RTL. 
In this example, there is one AIE kernel and three PL HLS kernels ``pl_kernels/mm2s.cpp``, ``pl_kernels/s2mm.cpp`` and ``pl_kernels/polar_clip.cpp`` compiled in functional mode. 

XO generation
--------------
During v++ compile flow, while creating hw_emu XO, the user provides a switch describing the intention to do a functional simulation 
that will generate XO with the SystemC wrapper on C code.

Input from the user
--------------------
1. Create a config file config <hw_emu_func.cfg> with following ``advanced`` compiler options: 

.. code:: cpp

   [advanced]
   param=compiler.emulationMode=func

2. In the Makefile, following flag is added in the v++ flow while compiling kernel:

::

      VPP_XO_FLAGS := --config hw_emu_func.cfg  

Speedup Benchmark   
------------------

Below is the runtime recorded for each suite of PL HLS kernels when compiled in functional mode vs the RTL. The speedup is observed as of 3x (approx). 
============================= ========================
     Kernel                    Runtime (s) 
============================= ========================
  mm2s-polar_clip-s2mm FUNC        127.8 
----------------------------- ------------------------   
  mm2s-polar_clip-s2mm RTL         414.26
----------------------------- ------------------------
  Speedup:                         3.24x  
============================= ======================== 
