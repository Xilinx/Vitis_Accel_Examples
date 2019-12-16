Multiple Compute Units (Asymmetrical) (C) 
======================

This is simple example of vector addition to demonstrate how to connect each compute unit to different banks and how to use these compute units in host applications

***KEY CONCEPTS:*** Multiple Compute Units

***KEYWORDS:*** #pragma HLS PIPELINE

## EXCLUDED PLATFORMS
Platforms containing following strings in their names are not supported for this example :
```
xilinx_u50_xdma
samsung
zc
```

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

