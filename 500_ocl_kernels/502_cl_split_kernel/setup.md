Large application Splits into Multiple Kernels(CL)
======================

This file contains the following sections:

1. SUPPORTED PLATFORMS
2. DESIGN FILES
3. COMMAND LINE ARGUMENTS


## 1. SUPPORTED PLATFORMS
Board | Device Name | Software Version
------|-------------|-----------------
Xilinx Alveo U250|xilinx_u250_xdma_201820_1|SDx 2018.3
Xilinx Virtex UltraScale+ VCU1525|xilinx_vcu1525_dynamic|SDx 2018.3
Xilinx Alveo U200|xilinx_u200_xdma_201820_1|SDx 2018.3


## 2. DESIGN FILES
Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

```
data/car_128x128.bmp
data/car_480x640.bmp
src/boost_helper.h
src/defns.h
src/host.cpp
src/median_helper.h
src/sketch.cl
src/sketch_bad.cl
src/sketch_helper.h
```

## 3. COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./host <sketch_GOOD XCLBIN> <sketch_BAD XCLBIN> ./data/car_128x128.bmp
```

