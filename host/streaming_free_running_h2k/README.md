Stream Free Running Host to Kernel(HLS C/C++)
======================

This is a simple streaming free running kernel that demonstrates how to interact with the host. In this design we showcase multiple stream requests in parallel.

***KEY CONCEPTS:*** Free Running Kernel

***KEYWORDS:*** cl_stream, CL_STREAM_EOT, CL_STREAM_NONBLOCKING

## EXCLUDED PLATFORMS
Platforms containing following strings in their names are not supported for this example :
```
zc
vck
xdma
qep
aws
samsung
```

##  DESIGN FILES
Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

```
src/host.cpp
src/increment.cpp
```

##  COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./host -x <increment XCLBIN>
```

##  COMMANDS FOR WINDOWS FLOW
Once the environment has been configured, run the following commands : 
```
cd cmake_build
cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Debug -DXILINX_XRT=<set xilinx xrt path> -DOCL_ROOT=<set ocl root path>
cmake --build . --verbose --config Debug --target install

For Example : 
cd cmake_build
cmake -G "Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Debug -DXILINX_XRT=C:\Xilinx\XRT -DOCL_ROOT=C:\Xilinx\XRT\ext
cmake --build . --verbose --config Debug --target install
```
