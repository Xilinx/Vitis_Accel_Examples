Global Memory Two Banks (C)
======================

This example of 2ddr is to demonstrate how to use multiple ddr and create buffers in each DDR.

***KEY CONCEPTS:*** Multiple Banks

***KEYWORDS:*** max_memory_ports, sp, connectivity

## EXCLUDED PLATFORMS
Platforms containing following strings in their names are not supported for this example :
```
zc
zed
xilinx_u50_xdma
samsung
```

##  DESIGN FILES
Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

```
data/golden.bmp
data/input.bmp
src/apply_watermark.cpp
src/host.cpp
```

##  COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./host <apply_watermark XCLBIN> ./data/input.bmp ./data/golden.bmp
```

