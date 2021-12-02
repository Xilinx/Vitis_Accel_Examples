RTL Kernel Examples
==================================
RTL Kernel Based Examples

 __Examples Table__ 

Example        | Description           | Key Concepts / Keywords 
---------------|-----------------------|-------------------------
[rtl_adder_streams/][]|This example shows an adder with streams using 3 RTL kernels.|__Key__ __Concepts__<br> - [RTL Kernel](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/RTL-Kernels)<br> - [Multiple RTL Kernels](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/RTL-Kernels)<br>
[rtl_streaming_free_running_k2k/][]|This is simple example which demonstrate how to use and configure a free running RTL kernel.|__Key__ __Concepts__<br> - [Read/Write Stream](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Host-Coding-for-Free-Running-Kernels)<br> - [RTL Kernel](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/RTL-Kernels)<br> - [Free Running Kernel](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Free-Running-Kernel)<br>__Keywords__<br> - [ap_axiu](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Streaming-Data-Transfers)<br> - [hls::stream](https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/HLS-Stream-Library)
[rtl_streaming_k2k_mm/][]|This example uses two simple increment RTL kernels connected to each other via stream interface and connected to host via memory mapped C++ data mover kernels|__Key__ __Concepts__<br> - [Read/Write Stream](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Host-Coding-for-Free-Running-Kernels)<br> - [RTL Kernel](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/RTL-Kernels)<br>__Keywords__<br> - [ap_axiu](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Streaming-Data-Transfers)<br> - [hls::stream](https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/HLS-Stream-Library)
[rtl_user_managed/][]|Simple example of user managed RTL Kernel.|__Key__ __Concepts__<br> - [User-Managed RTL Kernel](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Creating-User-Managed-RTL-Kernels)<br>__Keywords__<br> - [package_xo](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/package_xo-Command)<br> - [ctrl_protocol](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/package_xo-Command)<br> - [user_managed](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Creating-User-Managed-RTL-Kernels)<br> - [xrt::ip](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Setting-Up-User-Managed-Kernels-and-Argument-Buffers)<br> - [xrt::xclbin](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Specifying-the-Device-ID-and-Loading-the-XCLBIN)<br> - xrt::kernel::get_kernels<br> - xrt::kernel::get_cus<br> - xrt::kernel::get_args<br> - xrt::arg::get_offset<br> - xrt::ip::write_register<br> - xrt::ip::read_register
[rtl_vadd/][]|This is a simple example of vector addition using RTL kernel and supports all flows including sw_emu using C-Model.|__Key__ __Concepts__<br> - [RTL Kernel](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/RTL-Kernels)<br> - [RTL C-Model](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Software-Model-and-Host-Code-Example)<br>__Keywords__<br> - [kernel_files](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Packaging-the-RTL-Code-as-a-Vitis-XO)
[rtl_vadd_2clks/][]|This example shows vector addition with 2 kernel clocks using RTL Kernel.|__Key__ __Concepts__<br> - [RTL Kernel](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/RTL-Kernels)<br> - [Multiple Kernel Clocks](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/hls-Options)<br>__Keywords__<br> - [clock](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/clock-Options)<br> - [freqHz](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/clock-Options)<br> - [ap_clk](https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Control-Clock-and-Reset-in-AXI4-Lite-Interfaces)<br> - ap_clk2
[rtl_vadd_2kernels/][]|This example has two RTL Kernels. Both Kernel_0 and Kernel_1 perform vector addition. The Kernel_1 reads the output from Kernel_0 as one of two inputs.|__Key__ __Concepts__<br> - [Multiple RTL Kernels](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/RTL-Kernels)<br>
[rtl_vadd_hw_debug/][]|This is an example that showcases the Hardware Debug of Vector Addition RTL Kernel in Hardware.|__Key__ __Concepts__<br> - [RTL Kernel Debug](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Adding-Debug-IP-to-RTL-Kernels)<br>
[rtl_vadd_mixed_c_vadd/][]|This example has one RTL kernel and one C kernel. Both RTL kernel and C kernel perform vector addition. The C kernel reads the output from RTL kernel as one of two inputs.|__Key__ __Concepts__<br> - Mixed Kernels<br>

[.]:.
[rtl_adder_streams/]:rtl_adder_streams/
[rtl_streaming_free_running_k2k/]:rtl_streaming_free_running_k2k/
[rtl_streaming_k2k_mm/]:rtl_streaming_k2k_mm/
[rtl_user_managed/]:rtl_user_managed/
[rtl_vadd/]:rtl_vadd/
[rtl_vadd_2clks/]:rtl_vadd_2clks/
[rtl_vadd_2kernels/]:rtl_vadd_2kernels/
[rtl_vadd_hw_debug/]:rtl_vadd_hw_debug/
[rtl_vadd_mixed_c_vadd/]:rtl_vadd_mixed_c_vadd/
