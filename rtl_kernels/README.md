RTL Kernel Examples
==================================
RTL Kernel Based Examples

 __Examples Table__ 

Example        | Description           | Key Concepts / Keywords 
---------------|-----------------------|-------------------------
[rtl_adder_streams/][]|This example shows an adder with streams using 3 RTL kernels.|__Key__ __Concepts__<br> - [RTL Kernel](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devrtlkernel.html)<br> - [Multiple RTL Kernels](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devrtlkernel.html)<br>
[rtl_streaming_free_running_k2k/][]|This is simple example which demonstrate how to use and configure a free running RTL kernel.|__Key__ __Concepts__<br> - [Read/Write Stream](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/streamingconnections.html#ynb1556233012018)<br> - [RTL Kernel](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devrtlkernel.html)<br> - [Free Running Kernel](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/streamingconnections.html#uug1556136182736)<br>__Keywords__<br> - [ap_axiu](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/streamingconnections.html#tzq1555344621950)<br> - [hls::stream](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/hls_stream_library.html)
[rtl_streaming_k2k_mm/][]|This example uses two simple increment RTL kernels connected to each other via stream interface and connected to host via memory mapped C++ data mover kernels|__Key__ __Concepts__<br> - [Read/Write Stream](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/streamingconnections.html#ynb1556233012018)<br> - [RTL Kernel](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devrtlkernel.html)<br>__Keywords__<br> - [ap_axiu](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/streamingconnections.html#tzq1555344621950)<br> - [hls::stream](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/hls_stream_library.html)
[rtl_vadd/][]|Simple example of vector addition using RTL Kernel|__Key__ __Concepts__<br> - [RTL Kernel](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devrtlkernel.html)<br>
[rtl_vadd_2clks/][]|This example shows vector addition with 2 kernel clocks using RTL Kernel.|__Key__ __Concepts__<br> - [RTL Kernel](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devrtlkernel.html)<br> - [Multiple Kernel Clocks](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitiscommandcompiler.html#mcj1568640526180__section_bh5_dg4_bjb)<br>__Keywords__<br> - [kernel_frequency](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitiscommandcompiler.html#qcm1528577331870__section_frk_xtr_t3b)
[rtl_vadd_2kernels/][]|This example has two RTL Kernels. Both Kernel_0 and Kernel_1 perform vector addition. The Kernel_1 reads the output from Kernel_0 as one of two inputs.|__Key__ __Concepts__<br> - [Multiple RTL Kernels](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devrtlkernel.html)<br>
[rtl_vadd_hw_debug/][]|This is an example that showcases the Hardware Debug of Vector Addition RTL Kernel in Hardware.|__Key__ __Concepts__<br> - [RTL Kernel Debug](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/debuggingapplicationskernels.html#xey1524445482547)<br>
[rtl_vadd_mixed_c_vadd/][]|This example has one RTL kernel and one C kernel. Both RTL kernel and C kernel perform vector addition. The C kernel reads the output from RTL kernel as one of two inputs.|__Key__ __Concepts__<br> - Mixed Kernels<br>

[.]:.
[rtl_adder_streams/]:rtl_adder_streams/
[rtl_streaming_free_running_k2k/]:rtl_streaming_free_running_k2k/
[rtl_streaming_k2k_mm/]:rtl_streaming_k2k_mm/
[rtl_vadd/]:rtl_vadd/
[rtl_vadd_2clks/]:rtl_vadd_2clks/
[rtl_vadd_2kernels/]:rtl_vadd_2kernels/
[rtl_vadd_hw_debug/]:rtl_vadd_hw_debug/
[rtl_vadd_mixed_c_vadd/]:rtl_vadd_mixed_c_vadd/
