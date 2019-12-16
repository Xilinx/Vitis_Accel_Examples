RTL Stream Kernel to Kernel and Memory Mapped
======================

This example uses two simple increment RTL kernels connected to each other via stream interface and connected to host via memory mapped C++ data mover kernels

***KEY CONCEPTS:*** Read/Write Stream, RTL Kernel

***KEYWORDS:*** ap_axiu, hls::stream

##  DESIGN FILES
Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

```
src/gen_xo_myadder1.tcl
src/gen_xo_myadder2.tcl
src/host.cpp
src/krnl_mm2s.cpp
src/krnl_s2mm.cpp
```

##  COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./vadd_stream <myadder XCLBIN>
```

