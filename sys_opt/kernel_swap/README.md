Kernel Swap
======================

This example shows how host can swap the kernels and share same buffer between two kernels which are exist in separate binary containers. Dynamic platforms does not persist the buffer data so host has to migrate data from device to host memory before swapping the next kernel. After kernel swap, host has to migrate the buffer back to device.

***KEY CONCEPTS:*** Handling Buffer sharing across multiple binaries, Multiple Kernel Binaries

***KEYWORDS:*** enqueueMigrateMemObjects, CL_MIGRATE_MEM_OBJECT_HOST

## EXCLUDED PLATFORMS
Platforms containing following strings in their names are not supported for this example :
```
zc
```

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

