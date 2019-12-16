Local Memory Two Parallel Read/Write (C)
======================

This is simple example of vector addition to demonstrate how to utilize both ports of Local Memory.

***KEY CONCEPTS:*** Kernel Optimization, 2port BRAM Utilization, two read/write Local Memory

***KEYWORDS:*** #pragma HLS UNROLL FACTOR=2

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

