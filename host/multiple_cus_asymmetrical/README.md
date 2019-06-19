Vector Addition ~ Asymmetrical Multiple Compute Units (C) 
======================

This is simple example of vector addition to demonstrate how to connect each compute unit to different banks and how to use these compute units in host applications

***KEY CONCEPTS:*** Multiple Compute Units

***KEYWORDS:*** #pragma HLS PIPELINE

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
src/host.cpp
src/vadd.cpp
```

##  COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./host <vadd XCLBIN>
```

