Vadd 2 Kernel Clocks (RTL Kernel)
=================================

This example shows vector addition with 2 kernel clocks using RTL Kernel.

**KEY CONCEPTS:** `RTL Kernel <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/RTL-Kernels>`__, `Multiple Kernel Clocks <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/hls-Options>`__

**KEYWORDS:** `clock <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/clock-Options>`__, `freqHz <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/clock-Options>`__, `ap_clk <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Control-Clock-and-Reset-in-AXI4-Lite-Interfaces>`__, ap_clk2

**EXCLUDED PLATFORMS:** 

 - All NoDMA Platforms, i.e u50 nodma etc

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
following manner for SoC platforms -

::

   [clock]
   freqHz=150000000:krnl_vadd_2clk_rtl_1.ap_clk
   freqHz=250000000:krnl_vadd_2clk_rtl_1.ap_clk_2

The same is specified in the following manner for PCIe platforms -

::

   kernel_frequency=0:150|1:250

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.