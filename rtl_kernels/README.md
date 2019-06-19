RTL Kernel Examples
==================================
RTL Kernel Based Examples

 __Examples Table__ 

Example        | Description           | Key Concepts / Keywords 
---------------|-----------------------|-------------------------
[rtl_adder_streams/][]|This example shows an adder with streams using 3 RTL kernels.|__Key__ __Concepts__<br> - RTL Kernel<br> - Multiple RTL Kernels<br>
[rtl_streaming_free_running/][]|This is a simple Vector Increment RTL Kernel design with 1 Stream input and 1 Stream output that demonstrates on how to process an input stream of data for computation in an application.|__Key__ __Concepts__<br> - Read/Write Stream<br> - Create/Release Stream<br> - RTL Kernel<br>__Keywords__<br> - cl_stream<br> - CL_STREAM_EOT
[rtl_streaming_k2k/][]|This is a simple Vector Increment RTL Kernel to Kernel design with 1 Stream input and 1 Stream output that demonstrates on how to process an input stream of data for computation in an application.|__Key__ __Concepts__<br> - Read/Write Stream<br> - Create/Release Stream<br> - RTL Kernel<br>__Keywords__<br> - cl_stream<br> - CL_STREAM_EOT
[rtl_vadd/][]|Simple example of vector addition using RTL Kernel|__Key__ __Concepts__<br> - RTL Kernel<br>
[rtl_vadd_2clks/][]|This example shows vector addition with 2 kernel clocks using RTL Kernel.|__Key__ __Concepts__<br> - RTL Kernel<br> - Multiple Kernel Clocks<br>__Keywords__<br> - --kernel_frequency
[rtl_vadd_2kernels/][]|This example has two RTL Kernels. Both Kernel_0 and Kernel_1 perform vector addition. The Kernel_1 reads the output from Kernel_0 as one of two inputs.|__Key__ __Concepts__<br> - Multiple RTL Kernels<br>
[rtl_vadd_hw_debug/][]|This is an example that showcases the Hardware Debug of Vector Addition RTL Kernel in Hardware.|__Key__ __Concepts__<br> - RTL Kernel Debug<br>
[rtl_vadd_mixed_c_vadd/][]|This example has one RTL kernel and one C kernel. Both RTL kernel and C kernel perform vector addition. The C kernel reads the output from RTL kernel as one of two inputs.|__Key__ __Concepts__<br> - Mixed Kernels<br>

[.]:.
[rtl_adder_streams/]:rtl_adder_streams/
[rtl_streaming_free_running/]:rtl_streaming_free_running/
[rtl_streaming_k2k/]:rtl_streaming_k2k/
[rtl_vadd/]:rtl_vadd/
[rtl_vadd_2clks/]:rtl_vadd_2clks/
[rtl_vadd_2kernels/]:rtl_vadd_2kernels/
[rtl_vadd_hw_debug/]:rtl_vadd_hw_debug/
[rtl_vadd_mixed_c_vadd/]:rtl_vadd_mixed_c_vadd/
