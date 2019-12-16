Wide Memory Read/Write (CL)
======================

This is simple example of vector addition to demonstrate Wide Memory Access using uint16 data type. Based on input argument type, V++ compiler will figure our the memory datawidth between Global Memory and Kernel. For this example, uint16 datatype is used, so Memory datawidth will be 16 x (integer bit size) = 16 x 32 = 512 bit.

***KEY CONCEPTS:*** Kernel to DDR, wide memory access, burst read and write

***KEYWORDS:*** uint16, xcl_pipeline_loop

##  DESIGN FILES
Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

```
src/host.cpp
src/vadd.cl
```

##  COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./host <vadd XCLBIN>
```

