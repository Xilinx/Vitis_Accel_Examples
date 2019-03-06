RTL Kernel Examples
==================================
RTL Kernel Based Examples

 __Examples Table__ 

Example        | Description           | Key Concepts / Keywords 
---------------|-----------------------|-------------------------
[rtl_adder_pipes/][]|This example shows an adder with pipes using 3 RTL kernels.|__Key__ __Concepts__<br> - RTL Kernel<br> - Multiple RTL Kernels<br>
[rtl_vadd/][]|Simple example of vector addition using RTL Kernel|__Key__ __Concepts__<br> - RTL Kernel<br>
[rtl_vadd_2clks/][]|This example shows vector addition with 2 kernel clocks using RTL Kernel.|__Key__ __Concepts__<br> - RTL Kernel<br> - Multiple Kernel Clocks<br>__Keywords__<br> - --kernel_frequency
[rtl_vadd_2kernels/][]|This example has two RTL Kernels. Both Kernel_0 and Kernel_1 perform vector addition. The Kernel_1 reads the output from Kernel_0 as one of two inputs.|__Key__ __Concepts__<br> - Multiple RTL Kernels<br>
[rtl_vadd_hw_debug/][]|This is an example that showcases the Hardware Debug of Vector Addition RTL Kernel in Hardware.|__Key__ __Concepts__<br> - RTL Kernel Debug<br>
[rtl_vadd_mixed_c_vadd/][]|This example has one RTL kernel and one C kernel. Both RTL kernel and C kernel perform vector addition. The C kernel reads the output from RTL kernel as one of two inputs.|__Key__ __Concepts__<br> - Mixed Kernels<br>

[.]:.
[rtl_adder_pipes/]:rtl_adder_pipes/
[rtl_vadd/]:rtl_vadd/
[rtl_vadd_2clks/]:rtl_vadd_2clks/
[rtl_vadd_2kernels/]:rtl_vadd_2kernels/
[rtl_vadd_hw_debug/]:rtl_vadd_hw_debug/
[rtl_vadd_mixed_c_vadd/]:rtl_vadd_mixed_c_vadd/
