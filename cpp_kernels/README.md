HLS C/C++ Kernel Examples
==================================
This section contains HLS C/C++ Kernel Examples.

 __Examples Table__ 

Example        | Description           | Key Concepts / Keywords 
---------------|-----------------------|-------------------------
[00_cpu/][]|This is a simple example of matrix multiplication (Row x Col).|
[01_kernel/][]|This is a simple example of HLS matrix multiplication (Row x Col).|__Key__ __Concepts__<br> - OpenCL APIs<br>
[02_local_mem/][]|This is a simple example of matrix multiplication (Row x Col) to demonstrate how to reduce number of memory accesses using local memory.|__Key__ __Concepts__<br> - Kernel Optimization<br> - Local Memory<br>
[03_burst_rw/][]|This is a simple example of matrix multiplication (Row x Col) to demonstrate how to achieve better pipeline with burst read and write to/from local memory from/to DDR.|__Key__ __Concepts__<br> - Kernel Optimization<br> - Burst Read/Write<br>
[04_partition/][]|This is a simple example of matrix multiplication (Row x Col) to demonstrate how to achieve better performance by array partitioning and loop unrolling.|__Key__ __Concepts__<br> - Array Partition<br> - Loop Unroll<br>__Keywords__<br> - pragma HLS PIPELINE<br> - pragma HLS ARRAY_PARTITION complete<br> - pragma HLS UNROLL
[aos_vs_soa/][]|This example demonstrates how data layout can impact the performance of certain kernels. The example we will demonstrate how using the Structure of Array data layout can impact certain data parallel problems.|__Key__ __Concepts__<br> - Kernel Optimization<br> - Data Layout<br>
[array_partition/][]|This is a simple example of matrix multiplication (Row x Col) to demonstrate how to achieve better performance by array partitioning, using HLS kernel in SDx Environment.|__Key__ __Concepts__<br> - Kernel Optimization<br> - HLS C Kernel<br> - Array Partition<br>__Keywords__<br> - #pragma HLS ARRAY_PARTITION<br> - complete
[burst_rw/][]|This is simple example of using AXI4-master interface for burst read and write|__Key__ __Concepts__<br> - burst access<br>__Keywords__<br> - memcpy<br> - max_read_burst_length<br> - max_write_burst_length
[critical_path/][]|This example shows a normal coding style which could lead to critical path issue and design will give degraded timing.  Example also contains better coding style which can improve design timing.|__Key__ __Concepts__<br> - Critical Path handling<br> - Improve Timing<br>
[custom_datatype/][]|This is simple example of RGB to HSV conversion to demonstrate Custom DATA Type usages in C Based Kernel. Xilinx HLS Compiler Supports Custom Data Type to use for operation as well as Memory Interface between Kernel and Global Memory.|__Key__ __Concepts__<br> - Custom Datatype<br>__Keywords__<br> - struct<br> - #pragma HLS data_pack<br> - #pragma HLS LOOP_TRIPCOUNT
[dataflow_stream/][]|This is simple example of vector addition to demonstrate Dataflow functionality of HLS. HLS Dataflow allows user to schedule multiple task together to achieve higher throughput.|__Key__ __Concepts__<br> - Task Level Parallelism<br>__Keywords__<br> - dataflow<br> - hls::stream<>
[dataflow_stream_array/][]|This is simple example of Multiple Stages Vector Addition to demonstrate Array of Stream usage in HLS C Kernel Code.|__Key__ __Concepts__<br> - Array of Stream<br>__Keywords__<br> - dataflow<br> - hls::stream<>
[debug_profile/][]|This is simple example of vector addition and printing profile data (wall clock time taken between start and stop). It also dump a waveform file which can be reloaded to vivado to see the waveform. Run command 'vivado -source ./scripts/open_waveform.tcl -tclargs <device_name>-<kernel_name>.<target>.<device_name>.wdb' to launch waveform viewer. User can also update batch to gui in sdaccel.ini file to see the live waveform while running application.|__Key__ __Concepts__<br> - Use of Profile API<br> - Waveform Dumping and loading<br>
[dependence_inter/][]|This Example demonstrates the HLS pragma 'DEPENDENCE'.Using 'DEPENDENCE' pragma, user can provide additional dependency details to the compiler by specifying if the dependency in consecutive loop iterations on buffer is true/false, which allows the compiler to perform unrolling/pipelining to get better performance.|__Key__ __Concepts__<br> - Inter Dependence<br>__Keywords__<br> - DEPENDENCE<br> - inter<br> - WAR
[full_array_2d/][]|This is a simple example of accessing full data from 2d array|__Key__ __Concepts__<br> - 2D data full array Access<br>
[gmem_2banks/][]|This example of 2ddr to demonstrate on how to use 2ddr DSA. How to create buffers in each DDR.|__Key__ __Concepts__<br> - Multiple Banks<br>__Keywords__<br> - max_memory_ports<br> - misc:map_connect<br> - HLS Interface m_axi bundle
[gmem_2banks_5.0_shell/][]|This example of 2ddr to demonstrate on how to use 2ddr DSA. How to create buffers in each DDR.|__Key__ __Concepts__<br> - Multiple Banks<br>__Keywords__<br> - max_memory_ports<br> - misc:map_connect<br> - HLS Interface m_axi bundle
[kernel_global_bandwidth/][]|Bandwidth test of global to local memory.|
[kernel_global_bandwidth_5.0_shell/][]|Bandwidth test of global to local memory.|
[large_loop/][]|This is a CNN (Convolutional Neural Network) based example which mainly focuses on Convolution operation of a CNN network. The goal of this example is to demonstrate a method to overcome kernel design timing failure issue. It also presents the effectiveness of using multiple compute units to improve performance.|__Key__ __Concepts__<br> - Clock Frequency<br> - Multiple Compute Units<br> - Convolutional Neural Networks<br>__Keywords__<br> - #pragma HLS ARRAY_PARTITION<br> - #pragma HLS PIPELINE<br> - #pragma HLS INLINE
[lmem_2rw/][]|This is simple example of vector addition to demonstrate how to utilized both ports of Local Memory memory.|__Key__ __Concepts__<br> - Kernel Optimization<br> - 2port BRAM Utilization<br> - two read/write Local Memory<br>__Keywords__<br> - #pragma HLS UNROLL FACTOR=2
[loop_fusion/][]|This example will demonstrate how to fuse two loops into one to improve the performance of an OpenCL  C/C++ Kernel.|__Key__ __Concepts__<br> - Kernel Optimization<br> - Loop Fusion<br> - Loop Pipelining<br>__Keywords__<br> - #pragma HLS PIPELINE
[loop_pipeline/][]|This example demonstrates how loop pipelining can be used to improve the performance of a kernel.|__Key__ __Concepts__<br> - Kernel Optimization<br> - Loop Pipelining<br>__Keywords__<br> - pragma HLS PIPELINE
[loop_reorder/][]|This is a simple example of matrix multiplication (Row x Col) to demonstrate how to achieve better pipeline II factor by loop reordering.|__Key__ __Concepts__<br> - Kernel Optimization<br> - Loop reorder to improve II<br>__Keywords__<br> - #pragma HLS PIPELINE<br> - #pragma HLS ARRAY_PARTITION
[memcoalesce_hang/][]|This example shows Memory Coalesce Deadlock/Hand situation and how to handle it. User can switch between BAD and GOOD case using makefile variable KFLOW.|__Key__ __Concepts__<br> - Memory Coalesce<br> - Memory Deadlock/Hang<br> - Multiple Interfaces<br>__Keywords__<br> - HLS INTERFACE<br> - bundle<br> - m_axi
[partition_cyclicblock/][]|This example shows how to use array block and cyclic partitioning to improve performance of a kernel|__Key__ __Concepts__<br> - Kernel Optimization<br> - Array Partitioning<br> - Block Partition<br> - Cyclic Partition<br>__Keywords__<br> - #pragma HLS ARRAY_PARTITION<br> - cyclic<br> - block<br> - factor<br> - dim
[plram_access/][]|This example shows the usage of PLRAM and how to use it with simple matrix multiplication (Row x Col).|__Key__ __Concepts__<br> - SDx Memory Hierarchy<br> - PLRAMs<br>__Keywords__<br> - PLRAM
[row_array_2d/][]|This is a simple example of accessing each row of data from 2d array|__Key__ __Concepts__<br> - Row of 2D data array access<br>__Keywords__<br> - hls::stream
[shift_register/][]|This example demonstrates how to shift values in registers in each clock cycle|__Key__ __Concepts__<br> - Kernel Optimization<br> - Shift Register<br> - FIR<br>__Keywords__<br> - #pragma HLS ARRAY_PARTITION
[slr_assign/][]|This is simple example to describe SLR assignment information for a platform design. This example highlights how to provide extra input to assign the logic of the kernel into a nominated SLR. In this example we are assigning first kernel(Vector Multiplication) to SLR0 and assigning the second kernel(Vector Addition) to SLR1|__Key__ __Concepts__<br> - SLR Assignments<br>__Keywords__<br> - slr
[split_kernel/][]|This is a multi-filter image processing application to showcase effectiveness of Dataflow/Streams usage. This examples is intended to help developers to break down the complex kernels into multiple sub-functions using HLS Dataflow/Streams. It presents a way to concurrently execute multiple functions with better area utilization compared to a complex single kernel implementation. The main objective of this example is to showcase a way to build a optimal FPGA design which achieves maximum frequency with optimal resource utilization and achieves better performance compared to single complex kernel implementations.|__Key__ __Concepts__<br> - Dataflow<br> - Stream<br>__Keywords__<br> - #pragma HLS DATAFLOW<br> - hls::stream<br> - #pragma HLS INLINE<br> - #pragma HLS ARRAY_PARTITION<br> - #pragma HLS PIPELINE
[systolic_array/][]|This is a simple example of matrix multiplication (Row x Col) to help developers learn systolic array based algorithm design. Note : Systolic array based algorithm design is well suited for FPGA.|
[too_many_cu/][]|This is simple example of vector addition to demonstrate effectiveness of using single compute unit with heavy work load to achieve better performance. Bad example uses multiple compute units to achieve good performance but it results in heavy usage of FPGA resources and area due to which design fails timing. Good example uses single compute unit to compute with heavier work load, it helps in less resource utilization and also helps in kernel scalability. To switch between Good/Bad cases use the flag provided in makefile.|__Key__ __Concepts__<br> - Clock Frequency<br> - Data Level Parallelism<br> - Multiple Compute Units<br>__Keywords__<br> - #pragma HLS PIPELINE<br> - #pragma HLS ARRAY_PARTITION
[wide_mem_rw/][]|This is simple example of vector addition to demonstrate Wide Memory Access using ap_uint<512> data type. Based on input argument type, xocc compiler will figure our the memory datawidth between Global Memory and Kernel. For this example, ap_uint<512> datatype is used, so Memory datawidth will be 16 x (integer bit size) = 16 x 32 = 512 bit.|__Key__ __Concepts__<br> - Kernel to DDR<br> - wide memory access<br> - burst read and write<br>__Keywords__<br> - ap_uint<><br> - ap_int.h
[window_array_2d/][]|This is a simple example of accessing each window of data from 2d array|__Key__ __Concepts__<br> - window of 2D data array access<br>__Keywords__<br> - #pragma HLS DATAFLOW<br> - #pragma HLS PIPELINE<br> - #pragma HLS stream

[.]:.
[00_cpu/]:00_cpu/
[01_kernel/]:01_kernel/
[02_local_mem/]:02_local_mem/
[03_burst_rw/]:03_burst_rw/
[04_partition/]:04_partition/
[aos_vs_soa/]:aos_vs_soa/
[array_partition/]:array_partition/
[burst_rw/]:burst_rw/
[critical_path/]:critical_path/
[custom_datatype/]:custom_datatype/
[dataflow_stream/]:dataflow_stream/
[dataflow_stream_array/]:dataflow_stream_array/
[debug_profile/]:debug_profile/
[dependence_inter/]:dependence_inter/
[full_array_2d/]:full_array_2d/
[gmem_2banks/]:gmem_2banks/
[gmem_2banks_5.0_shell/]:gmem_2banks_5.0_shell/
[kernel_global_bandwidth/]:kernel_global_bandwidth/
[kernel_global_bandwidth_5.0_shell/]:kernel_global_bandwidth_5.0_shell/
[large_loop/]:large_loop/
[lmem_2rw/]:lmem_2rw/
[loop_fusion/]:loop_fusion/
[loop_pipeline/]:loop_pipeline/
[loop_reorder/]:loop_reorder/
[memcoalesce_hang/]:memcoalesce_hang/
[partition_cyclicblock/]:partition_cyclicblock/
[plram_access/]:plram_access/
[row_array_2d/]:row_array_2d/
[shift_register/]:shift_register/
[slr_assign/]:slr_assign/
[split_kernel/]:split_kernel/
[systolic_array/]:systolic_array/
[too_many_cu/]:too_many_cu/
[wide_mem_rw/]:wide_mem_rw/
[window_array_2d/]:window_array_2d/
