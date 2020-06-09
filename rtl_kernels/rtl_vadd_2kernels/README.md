Vadd 2 Kernels (RTL Kernel)
======================

This example has two RTL Kernels. Both Kernel_0 and Kernel_1 perform vector addition. The Kernel_1 reads the output from Kernel_0 as one of two inputs.

***KEY CONCEPTS:*** Multiple RTL Kernels

##  DESIGN FILES
Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

```
src/host.cpp
src/ip_0/hdl/krnl_vadd_rtl_0.v
src/ip_0/hdl/krnl_vadd_rtl_adder.sv
src/ip_0/hdl/krnl_vadd_rtl_axi_read_master.sv
src/ip_0/hdl/krnl_vadd_rtl_axi_write_master.sv
src/ip_0/hdl/krnl_vadd_rtl_control_s_axi.v
src/ip_0/hdl/krnl_vadd_rtl_counter.sv
src/ip_0/hdl/krnl_vadd_rtl_int.sv
src/ip_1/hdl/krnl_vadd_rtl_1.v
src/ip_1/hdl/krnl_vadd_rtl_adder.sv
src/ip_1/hdl/krnl_vadd_rtl_axi_read_master.sv
src/ip_1/hdl/krnl_vadd_rtl_axi_write_master.sv
src/ip_1/hdl/krnl_vadd_rtl_control_s_axi.v
src/ip_1/hdl/krnl_vadd_rtl_counter.sv
src/ip_1/hdl/krnl_vadd_rtl_int.sv
```

##  COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./host <vadd XCLBIN>
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
