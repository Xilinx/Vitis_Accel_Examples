Mixed Kernels (RTL & C)
======================

This example has one RTL kernel and one C kernel. Both RTL kernel and C kernel perform vector addition. The C kernel reads the output from RTL kernel as one of two inputs.

***KEY CONCEPTS:*** Mixed Kernels

## SUPPORTED SHELLS
Shell | Board             | Software Version
---------|-------------------|-----------------
xilinx_u200_qdma|Xilinx Alveo U200|VITIS 2019.2
xilinx_u200_xdma|Xilinx Alveo U200|VITIS 2019.2
xilinx_u250_qdma|Xilinx Alveo U250|VITIS 2019.2
xilinx_u250_xdma|Xilinx Alveo U250|VITIS 2019.2
xilinx_u280_xdma|Xilinx Alveo U280|VITIS 2019.2
xilinx_u50_xdma|Xilinx Alveo U50|VITIS 2019.2
zc702_base|Xilinx Zynq-7000 SoC ZC702|VITIS 2019.2
zc706_base|Xilinx Zynq-7000 SoC ZC706|VITIS 2019.2
zcu102_base|Xilinx UltraScale+ MPSoC ZCU102|VITIS 2019.2
zcu104_base|Xilinx UltraScale+ MPSoC ZCU104|VITIS 2019.2


##  DESIGN FILES
Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

```
src/hdl/krnl_vadd_rtl.v
src/hdl/krnl_vadd_rtl_adder.sv
src/hdl/krnl_vadd_rtl_axi_read_master.sv
src/hdl/krnl_vadd_rtl_axi_write_master.sv
src/hdl/krnl_vadd_rtl_control_s_axi.v
src/hdl/krnl_vadd_rtl_counter.sv
src/hdl/krnl_vadd_rtl_int.sv
src/host.cpp
src/kernel.xml
src/krnl_vadd.cpp
```

##  COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./host <vadd XCLBIN>
```

