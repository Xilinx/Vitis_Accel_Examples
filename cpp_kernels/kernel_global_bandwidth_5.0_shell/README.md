kernel_global
======================

Bandwidth test of global to local memory.

## SUPPORTED PLATFORMS
Platform | Board             | Software Version
---------|-------------------|-----------------
xilinx_aws-vu9p-f1-04261818|Xilinx Only 5.0 Shell|SDx 2019.1


##  DESIGN FILES
Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

```
src/kernel.cl
src/kernel_global_bandwidth.cpp
```

##  COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./kernel_global <krnl_kernel_global XCLBIN>
```

