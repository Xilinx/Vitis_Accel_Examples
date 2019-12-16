Systolic Array (C)
======================

This is a simple example of matrix multiplication (Row x Col) to help developers learn systolic array based algorithm design. Note : Systolic array based algorithm design is well suited for FPGA.

## EXCLUDED PLATFORMS
Platforms containing following strings in their names are not supported for this example :
```
zcu104_base
zc702_base
```

##  DESIGN FILES
Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

```
src/host.cpp
src/mmult.cpp
```

##  COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./host <mmult XCLBIN>
```

