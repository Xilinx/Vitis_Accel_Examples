HLS C/C++ Kernel Examples
==================================
This section contains HLS C/C++ Kernel Examples.

 __Examples Table__ 

Example        | Description           | Key Concepts / Keywords 
---------------|-----------------------|-------------------------
[array_partition/][]|This is a simple example of matrix multiplication (Row x Col) to demonstrate how to achieve better performance by array partitioning, using HLS kernel in Vitis Environment.|__Key__ __Concepts__<br> - Kernel Optimization<br> - HLS C Kernel<br> - Array Partition<br>__Keywords__<br> - #pragma HLS ARRAY_PARTITION<br> - complete
[bind_op_storage/][]|This is simple example of vector addition to describe how to use BIND OP and STORAGE for better implementation style.|__Key__ __Concepts__<br> - BIND OP<br> - BIND STORAGE<br>__Keywords__<br> - BIND_OP<br> - BIND_STORAGE<br> - impl<br> - op<br> - type<br> - latency
[burst_rw/][]|This is simple example of using AXI4-master interface for burst read and write|__Key__ __Concepts__<br> - burst access<br>__Keywords__<br> - memcpy<br> - max_read_burst_length<br> - max_write_burst_length
[critical_path/][]|This example shows a normal coding style which could lead to critical path issue and design will give degraded timing.  Example also contains better coding style which can improve design timing.|__Key__ __Concepts__<br> - Critical Path handling<br> - Improve Timing<br>
[custom_datatype/][]|This is simple example of RGB to HSV conversion to demonstrate Custom DATA Type usages in C Based Kernel. Xilinx HLS Compiler Supports Custom Data Type to use for operation as well as Memory Interface between Kernel and Global Memory.|__Key__ __Concepts__<br> - Custom Datatype<br>__Keywords__<br> - struct<br> - #pragma HLS LOOP_TRIPCOUNT
[dataflow_stream/][]|This is simple example of vector addition to demonstrate Dataflow functionality of HLS. HLS Dataflow allows user to schedule multiple task together to achieve higher throughput.|__Key__ __Concepts__<br> - Task Level Parallelism<br>__Keywords__<br> - dataflow<br> - hls::stream
[dataflow_stream_array/][]|This is simple example of Multiple Stages Vector Addition to demonstrate Array of Stream usage in HLS C Kernel Code.|__Key__ __Concepts__<br> - Array of Stream<br>__Keywords__<br> - dataflow<br> - hls::stream
[dependence_inter/][]|This Example demonstrates the HLS pragma 'DEPENDENCE'.Using 'DEPENDENCE' pragma, user can provide additional dependency details to the compiler by specifying if the dependency in consecutive loop iterations on buffer is true/false, which allows the compiler to perform unrolling/pipelining to get better performance.|__Key__ __Concepts__<br> - Inter Dependence<br>__Keywords__<br> - DEPENDENCE<br> - inter<br> - WAR
[gmem_2banks/][]|This example of 2ddr is to demonstrate how to use multiple ddr and create buffers in each DDR.|__Key__ __Concepts__<br> - Multiple Banks<br>__Keywords__<br> - max_memory_ports<br> - sp<br> - connectivity
[kernel_chain/][]|This is a kernel containing the cascaded Matrix Multiplication using dataflow. ap_ctrl_chain is enabled for this kernel to showcase how multiple enqueue of Kernel calls can be overlapped to give higher performance. ap_ctrl_chain allow kernel to start processing of next kernel operation before completing the current kernel operation.|__Key__ __Concepts__<br> - ap_ctrl_chain<br> - PLRAM<br>
[kernel_global_bandwidth/][]|Bandwidth test of global to local memory.|
[lmem_2rw/][]|This is simple example of vector addition to demonstrate how to utilize both ports of Local Memory.|__Key__ __Concepts__<br> - Kernel Optimization<br> - 2port BRAM Utilization<br> - two read/write Local Memory<br>__Keywords__<br> - #pragma HLS UNROLL FACTOR=2
[loop_pipeline/][]|This example demonstrates how loop pipelining can be used to improve the performance of a kernel.|__Key__ __Concepts__<br> - Kernel Optimization<br> - Loop Pipelining<br>__Keywords__<br> - pragma HLS PIPELINE
[loop_reorder/][]|This is a simple example of matrix multiplication (Row x Col) to demonstrate how to achieve better pipeline II factor by loop reordering.|__Key__ __Concepts__<br> - Kernel Optimization<br> - Loop reorder to improve II<br>__Keywords__<br> - #pragma HLS ARRAY_PARTITION
[partition_cyclicblock/][]|This example shows how to use array block and cyclic partitioning to improve performance of a kernel|__Key__ __Concepts__<br> - Kernel Optimization<br> - Array Partitioning<br> - Block Partition<br> - Cyclic Partition<br>__Keywords__<br> - #pragma HLS ARRAY_PARTITION<br> - cyclic<br> - block<br> - factor<br> - dim
[plram_access/][]|This example shows the usage of PLRAM and how to use it with simple matrix multiplication (Row x Col).|__Key__ __Concepts__<br> - Vitis Memory Hierarchy<br> - PLRAMs<br>__Keywords__<br> - PLRAM
[port_width_widening/][]|This example shows how HLS introduces the capability of resizing the port width of the kernel interface ports for better resource utilization maintaining the performance.|__Key__ __Concepts__<br> - Interface port width auto widening<br>__Keywords__<br> - m_axi_max_widen_bitwidth 
[shift_register/][]|This example demonstrates how to shift values in registers in each clock cycle|__Key__ __Concepts__<br> - Kernel Optimization<br> - Shift Register<br> - FIR<br>__Keywords__<br> - #pragma HLS ARRAY_PARTITION
[simple_vadd/][]|This is a simple example of vector addition.The purpose of this code is to introduce the user to application development in the Vitis tools.|
[systolic_array/][]|This is a simple example of matrix multiplication (Row x Col) to help developers learn systolic array based algorithm design. Note : Systolic array based algorithm design is well suited for FPGA.|
[wide_mem_rw/][]|This is simple example of vector addition to demonstrate Wide Memory Access using ap_uint<512> data type. Based on input argument type, V++ compiler will figure our the memory datawidth between Global Memory and Kernel. For this example, ap_uint<512> datatype is used, so Memory datawidth will be 16 x (integer bit size) = 16 x 32 = 512 bit.|__Key__ __Concepts__<br> - Kernel to DDR<br> - wide memory access<br> - burst read and write<br>__Keywords__<br> - ap_uint<><br> - ap_int.h

[.]:.
[array_partition/]:array_partition/
[bind_op_storage/]:bind_op_storage/
[burst_rw/]:burst_rw/
[critical_path/]:critical_path/
[custom_datatype/]:custom_datatype/
[dataflow_stream/]:dataflow_stream/
[dataflow_stream_array/]:dataflow_stream_array/
[dependence_inter/]:dependence_inter/
[gmem_2banks/]:gmem_2banks/
[kernel_chain/]:kernel_chain/
[kernel_global_bandwidth/]:kernel_global_bandwidth/
[lmem_2rw/]:lmem_2rw/
[loop_pipeline/]:loop_pipeline/
[loop_reorder/]:loop_reorder/
[partition_cyclicblock/]:partition_cyclicblock/
[plram_access/]:plram_access/
[port_width_widening/]:port_width_widening/
[shift_register/]:shift_register/
[simple_vadd/]:simple_vadd/
[systolic_array/]:systolic_array/
[wide_mem_rw/]:wide_mem_rw/
