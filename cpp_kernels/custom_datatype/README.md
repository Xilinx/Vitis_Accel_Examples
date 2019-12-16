Custom Data Type (C)
======================

This is simple example of RGB to HSV conversion to demonstrate Custom DATA Type usages in C Based Kernel. Xilinx HLS Compiler Supports Custom Data Type to use for operation as well as Memory Interface between Kernel and Global Memory.

***KEY CONCEPTS:*** Custom Datatype

***KEYWORDS:*** struct, #pragma HLS data_pack, #pragma HLS LOOP_TRIPCOUNT

##  DESIGN FILES
Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

```
data/input.bmp
src/host.cpp
src/rgb_to_hsv.cpp
src/rgb_to_hsv.h
```

##  COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./host <rgb_to_hsv XCLBIN> ./data/input.bmp
```

