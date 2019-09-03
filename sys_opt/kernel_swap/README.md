Vector Add - Vector Multiplication for XPR Devices
======================

This example shows how host can swap the kernels and share same buffer between two kernels which are exist in separate binary containers. Dynamic platforms does not persist the buffer data so host has to migrate data from device to host memory before swapping the next kernel. After kernel swap, host has to migrate the buffer back to device.

***KEY CONCEPTS:*** Handling Buffer sharing across multiple binaries, Multiple Kernel Binaries

***KEYWORDS:*** clEnqueueMigrateMemObjects(), CL_MIGRATE_MEM_OBJECT_HOST

## SUPPORTED SHELLS
Shell | Board             | Software Version
---------|-------------------|-----------------
xilinx_u200_qdma|Xilinx Alveo U200|VITIS 2019.2
xilinx_v350-es1_xdma|Xilinx Versal V350|VITIS 2019.2
xilinx_u280_xdma|Xilinx Alveo U280|VITIS 2019.2
xilinx_u250_qdma|Xilinx Alveo U250|VITIS 2019.2
xilinx_u200_xdma|Xilinx Alveo U200|VITIS 2019.2
xilinx_u250_xdma|Xilinx Alveo U250|VITIS 2019.2
xilinx_u280-es1_xdma|Xilinx Alveo U280|VITIS 2019.2


##  DESIGN FILES
Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

```
src/host.cpp
src/krnl_vadd.cpp
src/krnl_vmul.cpp
```

##  COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./vmulvadd <krnl_vmul XCLBIN> <krnl_vadd XCLBIN>
```

