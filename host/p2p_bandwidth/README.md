P2P byte copy Example
======================

This is simple example to test Synchronous and Asyncronous data transfer between SSD and FPGA.

***KEY CONCEPTS:*** P2P, SSD, XDMA

***KEYWORDS:*** XCL_MEM_EXT_P2P_BUFFER, pread, pwrite

## SUPPORTED PLATFORMS
Platform | Board             | Software Version
---------|-------------------|-----------------
xilinx_u200_qdma|Xilinx Alveo U200|VITIS 2019.2
xilinx_u200_xdma|Xilinx Alveo U200|VITIS 2019.2
xilinx_u250_qdma|Xilinx Alveo U250|VITIS 2019.2
xilinx_u250_xdma|Xilinx Alveo U250|VITIS 2019.2
xilinx_u280_xdma|Xilinx Alveo U280|VITIS 2019.2
xilinx_u50_xdma|Xilinx Alveo U50|VITIS 2019.2


##  DESIGN FILES
Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

```
src/host.cpp
src/copy0_kernel.cpp
src/copy1_kernel.cpp
src/xcl.c
src/xcl.h
```

##  COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./host <copy_kernel XCLBIN> -w ./sample.txt
```

