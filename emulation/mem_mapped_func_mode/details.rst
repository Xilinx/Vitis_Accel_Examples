Functional Hardware Emulation (HLS C/C++ MM Kernel)
===================================================

This example demonstrates the simulation of HLS C/C++ memory mapped kernel in functional mode. 
The functional mode in v++ generates the SCTLM code for the HLS kernel. 
HW Emulation is mainly targeted for HW Kernel Debug with detailed, cycle accurate view of kernel activity and 
functional model is an advanced use case when user wants to speedup the emulation by compiling desired kernels in functional mode. 
In this example, ``krnl_mmult1`` is compiled in functional mode and ``kernel_mmult2`` is compiled as default RTL during hardware emulation. 

XO generation
--------------
During v++ compile flow, while creating hw_emu XO, the user provides a switch describing the intention to do a functional simulation 
that will generate XO with the SystemC wrapper on C code.

Input from the user
--------------------
1. Create a config file <hw_emu_func.cfg> with following ``advanced`` compiler options: 

.. code:: cpp

   [advanced]
   param=compiler.emulationMode=func

2. In the Makefile, add the following flag in the v++ flow while compiling kernel:

::

      VPP_FLAGS_krnl_mmult1 += --config hw_emu_func.cfg 
      krnl_mmult1.xo: krnl_mmult1.cpp
      mkdir -p $(TEMP_DIR)
      $(VPP) $(VPP_FLAGS) $(VPP_FLAGS_krnl_mmult1) -c -k krnl_mmult1 --temp_dir $(TEMP_DIR)  -I'$(<D)' -o'$@' '$<'
