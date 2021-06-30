Vadd 2 Kernel Clocks (RTL Kernel)
=================================

This example shows vector addition with 2 kernel clocks using RTL Kernel.

**KEY CONCEPTS:** `RTL Kernel <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devrtlkernel.html>`__, `Multiple Kernel Clocks <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitiscommandcompiler.html#mcj1568640526180__section_bh5_dg4_bjb>`__

**KEYWORDS:** `kernel_frequency <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitiscommandcompiler.html#qcm1528577331870__section_frk_xtr_t3b>`__

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   zc
   vck
   nodma

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

DETAILS
-------

This example demonstrates the use of ``two kernel clocks`` for a simple
case of vector addition.

An RTL kernel can have up to two external clock interfaces; a primary
clock, ``ap_clk``, and an optional secondary clock, ``ap_clk_2``. Both
clocks can be used for clocking internal logic. However, all external
RTL kernel interfaces must be clocked on the primary clock. Both primary
and secondary clocks support independent automatic frequency scaling.

These clock frequencies are specified at V++ linking stage in the
following manner.

::

   kernel_frequency=0:150|1:250 

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.