Functional Hardware Emulation (Stream Free Running HLS C/C++ Kernel)
====================================================================

This example demonstrates how a user can run functional model of HLS kernel in HW Emulation.

**KEY CONCEPTS:** `Emulation Functional mode <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Working-with-Functional-Model-of-the-HLS-Kernel>`__

**KEYWORDS:** `emulationMode=func <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Working-with-Functional-Model-of-the-HLS-Kernel>`__

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/increment_func.cpp
   src/increment_rtl.cpp
   src/mem_read1.cpp
   src/mem_read2.cpp
   src/mem_write1.cpp
   src/mem_write2.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./stream_func_mode <krnl_incr XCLBIN>

DETAILS
-------

This example demonstrates the simulation of HLS C/C++ streaming free running kernel in functional mode. 
The functional mode in v++ generates the SCTLM code for the HLS kernel. 
HW Emulation is mainly targeted for HW Kernel Debug with detailed, cycle accurate view of kernel activity and 
functional model is an advanced use case when user wants to speedup the emulation by compiling desired kernels in functional mode. 
In this example, there are two stream free running HLS kernels out of which ``increment_func`` is compiled in functional mode 
and ``increment_rtl`` is compiled as default RTL during hardware emulation. 

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

2. In the Makefile, add the following flag in the v++ flow while compiling kernel:

::

      VPP_FLAGS_increment_func += --config hw_emu_func.cfg 
      increment_func.xo: increment_func.cpp
      mkdir -p $(TEMP_DIR)
      $(VPP) $(VPP_FLAGS) $(VPP_FLAGS_increment1) -c -k increment_func --temp_dir $(TEMP_DIR)  -I'$(<D)' -o'$@' '$<'

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.