Vector Addition with 2 Kernel Clocks (RTL)
======================

This example shows vector addition with 2 kernel clocks using RTL Kernel.

***KEY CONCEPTS:*** RTL Kernel, Multiple Kernel Clocks

***KEYWORDS:*** --kernel_frequency

## SUPPORTED PLATFORMS
Platform | Board             | Software Version
---------|-------------------|-----------------
xilinx_u200_qdma|Xilinx Alveo U200|SDx 2019.1
xilinx_u280_xdma|Xilinx Alveo U280|SDx 2019.1
xilinx_u250_qdma|Xilinx Alveo U250|SDx 2019.1
xilinx_u200_xdma|Xilinx Alveo U200|SDx 2019.1
xilinx_u250_xdma|Xilinx Alveo U250|SDx 2019.1
xilinx_u280-es1_xdma|Xilinx Alveo U280|SDx 2019.1


##  DESIGN FILES
Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

```
src/hdl/krnl_vadd_2clk_rtl.v
src/hdl/krnl_vadd_2clk_rtl_adder.sv
src/hdl/krnl_vadd_2clk_rtl_axi_read_master.sv
src/hdl/krnl_vadd_2clk_rtl_axi_write_master.sv
src/hdl/krnl_vadd_2clk_rtl_control_s_axi.v
src/hdl/krnl_vadd_2clk_rtl_counter.sv
src/hdl/krnl_vadd_2clk_rtl_int.sv
src/host.cpp
src/kernel.xml
```

##  COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./host <vadd XCLBIN>
```

