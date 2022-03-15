Functional Hardware Emulation (HLS C/C++ MM Kernel)
===================================================

This example demonstrates how a user can run functional model of HLS kernel in HW Emulation.

**KEY CONCEPTS:** Emulation Functional mode

**KEYWORDS:** emulationMode=func

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/krnl_mmult.hpp
   src/krnl_mmult_func.cpp
   src/krnl_mmult_rtl.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./mem_mapped_func_mode <krnl_mmult XCLBIN>

DETAILS
-------

This example demonstrates the simulation of HLS C/C++ memory mapped kernel in functional mode. 
The functional mode in v++ generates the SCTLM code for the HLS kernel. 
HW Emulation is mainly targeted for HW Kernel Debug with detailed, cycle accurate view of kernel activity and 
functional model is an advanced use case when user wants to speedup the emulation by compiling desired kernels in functional mode. 
In this example, ``krnl_mmult_func`` is compiled in functional mode and ``krnl_mmult_rtl`` is compiled as default RTL during hardware emulation. 

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

      VPP_FLAGS_krnl_mmult_func += --config hw_emu_func.cfg 
      krnl_mmult_func.xo: krnl_mmult_func.cpp
      mkdir -p $(TEMP_DIR)
      $(VPP) $(VPP_FLAGS) $(VPP_FLAGS_krnl_mmult_func) -c -k krnl_mmult_func --temp_dir $(TEMP_DIR)  -I'$(<D)' -o'$@' '$<'

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.