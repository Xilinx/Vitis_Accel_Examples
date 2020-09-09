Vadd 2 Kernel Clocks (RTL Kernel)
=================================

This example shows vector addition with 2 kernel clocks using RTL Kernel.

**KEY CONCEPTS:** RTL Kernel, Multiple Kernel Clocks

**KEYWORDS:** kernel_frequency

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/hdl/krnl_vadd_2clk_rtl.v
   src/hdl/krnl_vadd_2clk_rtl_adder.sv
   src/hdl/krnl_vadd_2clk_rtl_axi_read_master.sv
   src/hdl/krnl_vadd_2clk_rtl_axi_write_master.sv
   src/hdl/krnl_vadd_2clk_rtl_control_s_axi.v
   src/hdl/krnl_vadd_2clk_rtl_counter.sv
   src/hdl/krnl_vadd_2clk_rtl_int.sv
   src/host.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./rtl_vadd_2clks <vadd XCLBIN>

