Kernel To Global Memory Examples
==================================
Kernel to Global Memory Access Optimization.

 __Examples Table__ 

Example        | Description           | Key Concepts / Keywords 
---------------|-----------------------|-------------------------
[burst_rw_c/][]|This is simple example of using AXI4-master interface for burst read and write|__Key__ __Concepts__<br> - burst access<br>__Keywords__<br> - memcpy<br> - max_read_burst_length<br> - max_write_burst_length
[burst_rw_ocl/][]|This is simple example of using AXI4-master interface for burst read and write|__Key__ __Concepts__<br> - burst access<br>__Keywords__<br> - param:compiler.interfaceWrBurstLen<br> - param:compiler.interfaceRdBurstLen
[custom_datatype_c/][]|This is simple example of RGB to HSV conversion to demonstrate Custom DATA Type usages in C Based Kernel. Xilinx HLS Compiler Supports Custom Data Type to use for operation as well as Memory Interface between Kernel and Global Memory.|__Key__ __Concepts__<br> - Custom Datatype<br>__Keywords__<br> - struct<br> - #pragma HLS data_pack<br> - #pragma HLS LOOP_TRIPCOUNT
[full_array_2d_c/][]|This is a simple example of accessing full data from 2d array|__Key__ __Concepts__<br> - 2D data full array Access<br>
[full_array_2d_ocl/][]|This is a simple example of accessing full data from 2d array|__Key__ __Concepts__<br> - 2D data full array Access<br>
[gmem_2banks_c/][]|This example of 2ddr to demonstrate on how to use 2ddr DSA. How to create buffers in each DDR.|__Key__ __Concepts__<br> - Multiple Banks<br>__Keywords__<br> - max_memory_ports<br> - misc:map_connect<br> - XCL_MEM_DDR_BANK0<br> - XCL_MEM_DDR_BANK1<br> - XCL_MEM_DDR_BANKx<br> - HLS Interface m_axi bundle
[gmem_2banks_ocl/][]|This example of 2ddr to demonstrate on how to use 2ddr DSA. How to create buffers in each DDR.|__Key__ __Concepts__<br> - Multiple Banks<br>__Keywords__<br> - max_memory_ports<br> - misc:map_connect<br> - XCL_MEM_DDR_BANK0<br> - XCL_MEM_DDR_BANK1<br> - XCL_MEM_DDR_BANKx
[kernel_global_bandwidth/][]|Bandwidth test of global to local memory.|
[memcoalesce_hang_c/][]|This example shows Memory Coalesce Deadlock/Hand situation and how to handle it. User can switch between BAD and GOOD case using makefile variable KFLOW.|__Key__ __Concepts__<br> - Memory Coalesce<br> - Memory Deadlock/Hang<br> - Multiple Interfaces<br>__Keywords__<br> - HLS INTERFACE<br> - bundle<br> - m_axi
[plram_access_c/][]|This example shows the usage of PLRAM and how to use it with simple matrix multiplication (Row x Col).|__Key__ __Concepts__<br> - SDx Memory Hierarchy<br> - PLRAMs<br>__Keywords__<br> - PLRAM
[row_array_2d_c/][]|This is a simple example of accessing each row of data from 2d array|__Key__ __Concepts__<br> - Row of 2D data array access<br>__Keywords__<br> - hls::stream
[row_array_2d_ocl/][]|This is a simple example of accessing each row of data from 2d array|__Key__ __Concepts__<br> - Row of 2D data array access<br>__Keywords__<br> - xcl_dataflow<br> - xcl_pipeline_loop
[slr_assign/][]|This is simple example to describe SLR assignment information for a platform design. This example highlights how to provide extra input to assign the logic of the kernel into a nominated SLR. In this example we are assigning first kernel(Vector Multiplication) to SLR0 and assigning the second kernel(Vector Addition) to SLR1|__Key__ __Concepts__<br> - SLR Assignments<br>__Keywords__<br> - slr
[wide_mem_rw_c/][]|This is simple example of vector addition to demonstrate Wide Memory Access using ap_uint<512> data type. Based on input argument type, xocc compiler will figure our the memory datawidth between Global Memory and Kernel. For this example, ap_uint<512> datatype is used, so Memory datawidth will be 16 x (integer bit size) = 16 x 32 = 512 bit.|__Key__ __Concepts__<br> - Kernel to DDR<br> - wide memory access<br> - burst read and write<br>__Keywords__<br> - ap_uint<><br> - ap_int.h
[wide_mem_rw_ocl/][]|This is simple example of vector addition to demonstrate Wide Memory Access using uint16 data type. Based on input argument type, xocc compiler will figure our the memory datawidth between Global Memory and Kernel. For this example, uint16 datatype is used, so Memory datawidth will be 16 x (integer bit size) = 16 x 32 = 512 bit.|__Key__ __Concepts__<br> - Kernel to DDR<br> - wide memory access<br> - burst read and write<br>__Keywords__<br> - uint16<br> - xcl_pipeline_loop
[window_array_2d_c/][]|This is a simple example of accessing each window of data from 2d array|__Key__ __Concepts__<br> - window of 2D data array access<br>__Keywords__<br> - #pragma HLS DATAFLOW<br> - #pragma HLS PIPELINE<br> - #pragma HLS stream

[.]:.
[burst_rw_c/]:burst_rw_c/
[burst_rw_ocl/]:burst_rw_ocl/
[custom_datatype_c/]:custom_datatype_c/
[full_array_2d_c/]:full_array_2d_c/
[full_array_2d_ocl/]:full_array_2d_ocl/
[gmem_2banks_c/]:gmem_2banks_c/
[gmem_2banks_ocl/]:gmem_2banks_ocl/
[kernel_global_bandwidth/]:kernel_global_bandwidth/
[memcoalesce_hang_c/]:memcoalesce_hang_c/
[plram_access_c/]:plram_access_c/
[row_array_2d_c/]:row_array_2d_c/
[row_array_2d_ocl/]:row_array_2d_ocl/
[slr_assign/]:slr_assign/
[wide_mem_rw_c/]:wide_mem_rw_c/
[wide_mem_rw_ocl/]:wide_mem_rw_ocl/
[window_array_2d_c/]:window_array_2d_c/
