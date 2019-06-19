HBM Bandwidth
======================

This is a HBM bandwidth check design. Design contains 8 compute units of a kernel which has access to all HBM banks (0:31). Host application allocate buffer into all HBM banks and run these 8 compute units concurrently and measure the overall bandwidth between Kernel and HBM Memory.

## SUPPORTED PLATFORMS
Platform | Board             | Software Version
---------|-------------------|-----------------
xilinx_u280_xdma|Xilinx Alveo U280|SDx 2019.1
xilinx_u280-es1_xdma|Xilinx Alveo U280|SDx 2019.1


##  DESIGN FILES
Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

```
src/host.cpp
src/krnl_vaddmul.cpp
src/krnl_vaddmul.h
```

##  COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./host <krnl_vaddmul XCLBIN>
```

