P2P Simple Example
======================

This is simple example of vector increment to describe P2P between FPGA and NVMe SSD.

***KEY CONCEPTS:*** P2P, NVMe SSD, SmartSSD

***KEYWORDS:*** XCL_MEM_EXT_P2P_BUFFER, pread, pwrite, O_DIRECT, O_RDWR

## EXCLUDED PLATFORMS
Platforms containing following strings in their names are not supported for this example :
```
zc
u200
u250
u280
u50
```

##  DESIGN FILES
Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

```
src/adder.cpp
src/host.cpp
```

##  COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./host <adder XCLBIN> /dev/nvme0n1
```

