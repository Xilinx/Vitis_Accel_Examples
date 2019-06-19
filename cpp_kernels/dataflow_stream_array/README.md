Dataflow Using HLS Stream (C)
======================

This is simple example of Multiple Stages Vector Addition to demonstrate Array of Stream usage in HLS C Kernel Code.

***KEY CONCEPTS:*** Array of Stream

***KEYWORDS:*** dataflow, hls::stream<>

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
src/N_stage_adders.cpp
src/host.cpp
```

##  COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./host <N_stage_Adders XCLBIN>
```

