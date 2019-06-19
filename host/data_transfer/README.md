Data Transfer (C)
======================

This example illustrates several ways to use the OpenCL API to transfer data to and from the FPGA

***KEY CONCEPTS:*** OpenCL API, Data Transfer, Write Buffers, Read Buffers, Map Buffers, Async Memcpy

***KEYWORDS:*** enqueueWriteBuffer(), enqueueReadBuffer(), enqueueMapBuffer(), enqueueUnmapMemObject(), enqueueMigrateMemObjects()

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
src/dummy_kernel.cpp
src/host.cpp
```

##  COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./data_transfer <dummy_kernel XCLBIN>
```

