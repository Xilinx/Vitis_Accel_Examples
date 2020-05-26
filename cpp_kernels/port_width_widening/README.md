Port Width Widening
======================

This example shows how HLS introduces the capability of resizing the port width of the kernel interface ports for better resource utilization maintaining the performance.

***KEY CONCEPTS:*** Interface port width auto widening

***KEYWORDS:*** m_axi_max_widen_bitwidth 

##  DESIGN FILES
Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

```
src/host.cpp
src/krnl_base.cpp
src/krnl_widen.cpp
```

##  COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./host <krnl_port_widen XCLBIN>
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
