04 Matrix Multiplication Array Partition and Loop Unroll
======================

This is a simple example of matrix multiplication (Row x Col) to demonstrate how to achieve better performance by array partitioning and loop unrolling.

***KEY CONCEPTS:*** Array Partition, Loop Unroll

***KEYWORDS:*** pragma HLS PIPELINE, pragma HLS ARRAY_PARTITION complete, pragma HLS UNROLL

## SUPPORTED PLATFORMS
Platform | Board             | Software Version
---------|-------------------|-----------------
xilinx_u200_qdma|Xilinx Alveo U200|SCOUT 2019.2
xilinx_u280_xdma|Xilinx Alveo U280|SCOUT 2019.2
xilinx_u250_qdma|Xilinx Alveo U250|SCOUT 2019.2
xilinx_u200_xdma|Xilinx Alveo U200|SCOUT 2019.2
xilinx_u250_xdma|Xilinx Alveo U250|SCOUT 2019.2
xilinx_u280-es1_xdma|Xilinx Alveo U280|SCOUT 2019.2


##  DESIGN FILES
Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

```
src/host.cpp
src/mmult.cpp
```

##  COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./host <mmult XCLBIN>
```

