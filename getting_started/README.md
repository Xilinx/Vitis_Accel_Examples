Getting Started Examples
==================================
This page contains examples for users who are new to Xilinx SDx OpenCL Flows. The focus of the examples is towards code optimization for Xilinx devices.The table lists various categories of examples in suggested order which users can follow.


__Prerequisites__
 - User is familiar with basics of OpenCL flow.
 - User has gone through SDx tutorial and is familiar with basics of tool functionality and terminology.

S.No.   | Category  | Description 
--------|-----------|-----------------------------------------
1 | [hello_world][]      |Hello World examples for new users
2 | [cpu_to_fpga][]      |Labs to showcase the cpu to fpga conversion with kernel optimizations.
3 | [host][]      |OpenCL host code for optimized interfacing with Xilinx Devices
4 | [kernel_to_gmem][]      |Kernel to Global Memory Access Optimization.
5 | [kernel_opt][]      |Kernel Optimization for performance
6 | [dataflow][]      |Kernel Optimization through Macro Level Pipelining
7 | [clk_freq][]      |Improving Kernel Clock Frequency through Optimized code.
8 | [debug][]      |Debugging and Profiling of Kernel.
9 | [rtl_kernel][]      |RTL Kernel Based Examples

 __Examples Table__ 

Example        | Description           | Key Concepts / Keywords 
---------------|-----------------------|-------------------------
[hello_world/helloworld_c/][]|This is simple example of vector addition to describe how to use HLS kernels in Sdx Environment. This example highlights the concepts like PIPELINE which increases the kernel performance |__Key__ __Concepts__<br> - HLS C Kernel<br> - OpenCL Host APIs<br>__Keywords__<br> - gmem<br> - bundle<br> - #pragma HLS INTERFACE<br> - m_axi<br> - s_axi4lite
[hello_world/helloworld_ocl/][]|This example is a simple OpenCL application. It will highlight the basic flow of an OpenCL application.|__Key__ __Concepts__<br> - OpenCL API<br>
[cpu_to_fpga/00_cpu/][]|This is a simple example of matrix multiplication (Row x Col).|
[cpu_to_fpga/01_ocl/][]|This is a simple example of OpenCL matrix multiplication (Row x Col).|__Key__ __Concepts__<br> - OpenCL APIs<br>
[cpu_to_fpga/02_lmem_ocl/][]|This is a simple example of matrix multiplication (Row x Col) to demonstrate how to reduce number of memory accesses using local memory.|__Key__ __Concepts__<br> - Kernel Optimization<br> - Local Memory<br>
[cpu_to_fpga/03_burst_rw_ocl/][]|This is a simple example of matrix multiplication (Row x Col) to demonstrate how to achieve better pipeline with burst read and write to/from local memory from/to DDR.|__Key__ __Concepts__<br> - Kernel Optimization<br> - Burst Read/Write<br>
[cpu_to_fpga/04_partition_ocl/][]|This is a simple example of matrix multiplication (Row x Col) to demonstrate how to achieve better performance by array partitioning and loop unrolling.|__Key__ __Concepts__<br> - Array Partition<br> - Loop Unroll<br>__Keywords__<br> - xcl_pipeline_loop<br> - xcl_array_partition(complete, dim)<br> - opencl_unroll_hint
[host/concurrent_kernel_execution_c/][]|This example will demonstrate how to use multiple and out of order command queues to simultaneously execute multiple kernels on an FPGA.|__Key__ __Concepts__<br> - Concurrent execution<br> - Out of Order Command Queues<br> - Multiple Command Queues<br>__Keywords__<br> - CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE<br> - clSetEventCallback()
[host/copy_buffer_c/][]|This Copy Buffer example demonstrate how one buffer can be copied from another buffer.|__Key__ __Concepts__<br> - Copy Buffer<br>__Keywords__<br> - cl::CommandQueue::enqueueCopyBuffer()
[host/data_transfer_c/][]|This example illustrates several ways to use the OpenCL API to transfer data to and from the FPGA|__Key__ __Concepts__<br> - OpenCL API<br> - Data Transfer<br> - Write Buffers<br> - Read Buffers<br> - Map Buffers<br> - Async Memcpy<br>__Keywords__<br> - enqueueWriteBuffer()<br> - enqueueReadBuffer()<br> - enqueueMapBuffer()<br> - enqueueUnmapMemObject()<br> - enqueueMigrateMemObjects()
[host/device_query_c/][]|This example prints the OpenCL properties of the platform and its devices. It also displays the limits and capabilities of the hardware.|__Key__ __Concepts__<br> - OpenCL API<br> - Querying device properties<br>__Keywords__<br> - clGetPlatformIDs()<br> - clGetPlatformInfo()<br> - clGetDeviceIDs()<br> - clGetDeviceInfo()
[host/device_query_cpp/][]|This Example prints the OpenCL properties of the platform and its devices using OpenCL CPP APIs. It also displays the limits and capabilities of the hardware.|__Key__ __Concepts__<br> - OpenCL API<br> - Querying device properties<br>
[host/errors_c/][]|This example discuss the different reasons for errors in OpenCL and how to handle them at runtime.|__Key__ __Concepts__<br> - OpenCL API<br> - Error handling<br>__Keywords__<br> - CL_SUCCESS<br> - CL_DEVICE_NOT_FOUND<br> - CL_DEVICE_NOT_AVAILABLE
[host/errors_cpp/][]|This example discuss the different reasons for errors in OpenCL C++ and how to handle them at runtime.|__Key__ __Concepts__<br> - OpenCL C++ API<br> - Error handling<br>__Keywords__<br> - CL_SUCCESS<br> - CL_DEVICE_NOT_FOUND<br> - CL_DEVICE_NOT_AVAILABLE<br> - CL_INVALID_VALUE<br> - CL_INVALID_KERNEL_NAME<br> - CL_INVALID_BUFFER_SIZE
[host/host_global_bandwidth/][]|Host to global memory bandwidth test|
[host/kernel_swap_c/][]|This example shows how host can swap the kernels and share same buffer between two kernels which are exist in separate binary containers. Dynamic platforms does not persist the buffer data so host has to migrate data from device to host memory before swapping the next kernel. After kernel swap, host has to migrate the buffer back to device.|__Key__ __Concepts__<br> - Handling Buffer sharing across multiple binaries<br> - Multiple Kernel Binaries<br>__Keywords__<br> - clEnqueueMigrateMemObjects()<br> - CL_MIGRATE_MEM_OBJECT_HOST
[host/multiple_devices_c/][]|This example show how to take advantage of multiple FPGAs on a system. It will show how to initialized an OpenCL context, allocate memory on the two devices and execute a kernel on each FPGA.|__Key__ __Concepts__<br> - OpenCL API<br> - Multi-FPGA Execution<br> - Event Handling<br>__Keywords__<br> - cl_device_id<br> - clGetDeviceIDs()
[host/multiple_process_c/][]|This example will demonstrate how to run multiple processes to utilize multiple kernels simultaneously on an FPGA device. Multiple processes can share access to the same device provided each process uses the same xclbin. Processes share access to all device resources but there is no support for exclusive access to resources by any process.|__Key__ __Concepts__<br> - Concurrent execution<br> - Multiple HLS kernels<br> - Multiple Process Support<br>__Keywords__<br> - PID<br> - fork<br> - XCL_MULTIPROCESS_MODE<br> - multiprocess
[host/overlap_c/][]|This examples demonstrates techniques that allow user to overlap Host(CPU) and FPGA computation in an application. It will cover asynchronous operations and event object.|__Key__ __Concepts__<br> - OpenCL API<br> - Synchronize Host and FPGA<br> - Asynchronous Processing<br> - Events<br> - Asynchronous memcpy<br>__Keywords__<br> - cl_event<br> - clCreateCommandQueue<br> - CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE<br> - clEnqueueMigrateMemObjects
[host/stream_access_c/][]|This is a simple example that demonstrates on how to process an input stream of data for computation in an application. It shows how to perform asynchronous operations and event handling.|__Key__ __Concepts__<br> - OpenCL API<br> - Synchronize Host and FPGA<br> - Asynchronous Processing<br> - Events<br> - Asynchronous Data Transfer<br>__Keywords__<br> - cl::event<br> - CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE
[host/sub_devices_c/][]|This example demonstrates how to create OpenCL subdevices which uses a single kernel multiple times in order to show how to handle each instance independently including independent buffers, command queues and sequencing.|__Key__ __Concepts__<br> - Sub Devices<br>__Keywords__<br> - cl_device_partition_property<br> - createSubDevices<br> - CL_DEVICE_PARTITION_EQUALLY
[kernel_to_gmem/burst_rw_c/][]|This is simple example of using AXI4-master interface for burst read and write|__Key__ __Concepts__<br> - burst access<br>__Keywords__<br> - memcpy<br> - max_read_burst_length<br> - max_write_burst_length
[kernel_to_gmem/burst_rw_ocl/][]|This is simple example of using AXI4-master interface for burst read and write|__Key__ __Concepts__<br> - burst access<br>__Keywords__<br> - param:compiler.interfaceWrBurstLen<br> - param:compiler.interfaceRdBurstLen
[kernel_to_gmem/custom_datatype_c/][]|This is simple example of RGB to HSV conversion to demonstrate Custom DATA Type usages in C Based Kernel. Xilinx HLS Compiler Supports Custom Data Type to use for operation as well as Memory Interface between Kernel and Global Memory.|__Key__ __Concepts__<br> - Custom Datatype<br>__Keywords__<br> - struct<br> - #pragma HLS data_pack<br> - #pragma HLS LOOP_TRIPCOUNT
[kernel_to_gmem/custom_datatype_ocl/][]|This is simple example of RGB to HSV conversion to demonstrate Custom DATA Type usages in OpenCL Based Kernel. Xilinx HLS Compiler Supports Custom Data Type to use for operation as well as Memory Interface between Kernel and Global Memory.|__Key__ __Concepts__<br> - Dataflow<br> - Custom Datatype<br>__Keywords__<br> - struct
[kernel_to_gmem/full_array_2d_c/][]|This is a simple example of accessing full data from 2d array|__Key__ __Concepts__<br> - 2D data full array Access<br>
[kernel_to_gmem/full_array_2d_ocl/][]|This is a simple example of accessing full data from 2d array|__Key__ __Concepts__<br> - 2D data full array Access<br>
[kernel_to_gmem/gmem_2banks_c/][]|This example of 2ddr to demonstrate on how to use 2ddr DSA. How to create buffers in each DDR.|__Key__ __Concepts__<br> - Multiple Banks<br>__Keywords__<br> - max_memory_ports<br> - misc:map_connect<br> - XCL_MEM_DDR_BANK0<br> - XCL_MEM_DDR_BANK1<br> - XCL_MEM_DDR_BANKx<br> - HLS Interface m_axi bundle
[kernel_to_gmem/gmem_2banks_ocl/][]|This example of 2ddr to demonstrate on how to use 2ddr DSA. How to create buffers in each DDR.|__Key__ __Concepts__<br> - Multiple Banks<br>__Keywords__<br> - max_memory_ports<br> - misc:map_connect<br> - XCL_MEM_DDR_BANK0<br> - XCL_MEM_DDR_BANK1<br> - XCL_MEM_DDR_BANKx
[kernel_to_gmem/kernel_global_bandwidth/][]|Bandwidth test of global to local memory.|
[kernel_to_gmem/memcoalesce_hang_c/][]|This example shows Memory Coalesce Deadlock/Hand situation and how to handle it. User can switch between BAD and GOOD case using makefile variable KFLOW.|__Key__ __Concepts__<br> - Memory Coalesce<br> - Memory Deadlock/Hang<br> - Multiple Interfaces<br>__Keywords__<br> - HLS INTERFACE<br> - bundle<br> - m_axi
[kernel_to_gmem/plram_access_c/][]|This example shows the usage of PLRAM and how to use it with simple matrix multiplication (Row x Col).|__Key__ __Concepts__<br> - SDx Memory Hierarchy<br> - PLRAMs<br>__Keywords__<br> - PLRAM
[kernel_to_gmem/row_array_2d_c/][]|This is a simple example of accessing each row of data from 2d array|__Key__ __Concepts__<br> - Row of 2D data array access<br>__Keywords__<br> - hls::stream
[kernel_to_gmem/row_array_2d_ocl/][]|This is a simple example of accessing each row of data from 2d array|__Key__ __Concepts__<br> - Row of 2D data array access<br>__Keywords__<br> - xcl_dataflow<br> - xcl_pipeline_loop
[kernel_to_gmem/slr_assign/][]|This is simple example to describe SLR assignment information for a platform design. This example highlights how to provide extra input to assign the logic of the kernel into a nominated SLR. In this example we are assigning first kernel(Vector Multiplication) to SLR0 and assigning the second kernel(Vector Addition) to SLR1|__Key__ __Concepts__<br> - SLR Assignments<br>__Keywords__<br> - slr
[kernel_to_gmem/wide_mem_rw_c/][]|This is simple example of vector addition to demonstrate Wide Memory Access using ap_uint<512> data type. Based on input argument type, xocc compiler will figure our the memory datawidth between Global Memory and Kernel. For this example, ap_uint<512> datatype is used, so Memory datawidth will be 16 x (integer bit size) = 16 x 32 = 512 bit.|__Key__ __Concepts__<br> - Kernel to DDR<br> - wide memory access<br> - burst read and write<br>__Keywords__<br> - ap_uint<><br> - ap_int.h
[kernel_to_gmem/wide_mem_rw_ocl/][]|This is simple example of vector addition to demonstrate Wide Memory Access using uint16 data type. Based on input argument type, xocc compiler will figure our the memory datawidth between Global Memory and Kernel. For this example, uint16 datatype is used, so Memory datawidth will be 16 x (integer bit size) = 16 x 32 = 512 bit.|__Key__ __Concepts__<br> - Kernel to DDR<br> - wide memory access<br> - burst read and write<br>__Keywords__<br> - uint16<br> - xcl_pipeline_loop
[kernel_to_gmem/window_array_2d_c/][]|This is a simple example of accessing each window of data from 2d array|__Key__ __Concepts__<br> - window of 2D data array access<br>__Keywords__<br> - #pragma HLS DATAFLOW<br> - #pragma HLS PIPELINE<br> - #pragma HLS stream
[kernel_to_gmem/window_array_2d_ocl/][]|This is a simple example of accessing each window of data from 2d array|__Key__ __Concepts__<br> - window/tile of 2D data array access<br>__Keywords__<br> - pipe<br> - xcl_pipeline_loop<br> - xcl_reqd_pipe_depth
[kernel_opt/aos_vs_soa_ocl/][]|This example demonstrates how data layout can impact the performance of certain kernels. The example we will demonstrate how using the Structure of Array data layout can impact certain data parallel problems.|__Key__ __Concepts__<br> - Kernel Optimization<br> - Data Layout<br>
[kernel_opt/array_partition_c/][]|This is a simple example of matrix multiplication (Row x Col) to demonstrate how to achieve better performance by array partitioning, using HLS kernel in SDx Environment.|__Key__ __Concepts__<br> - Kernel Optimization<br> - HLS C Kernel<br> - Array Partition<br>__Keywords__<br> - #pragma HLS ARRAY_PARTITION<br> - complete
[kernel_opt/array_partition_ocl/][]|This example shows how to use array partitioning to improve performance of a kernel|__Key__ __Concepts__<br> - Kernel Optimization<br> - Array Partitioning<br>__Keywords__<br> - xcl_array_partition<br> - complete
[kernel_opt/dependence_inter_c/][]|This Example demonstrates the HLS pragma 'DEPENDENCE'.Using 'DEPENDENCE' pragma, user can provide additional dependency details to the compiler by specifying if the dependency in consecutive loop iterations on buffer is true/false, which allows the compiler to perform unrolling/pipelining to get better performance.|__Key__ __Concepts__<br> - Inter Dependence<br>__Keywords__<br> - DEPENDENCE<br> - inter<br> - WAR
[kernel_opt/lmem_2rw_c/][]|This is simple example of vector addition to demonstrate how to utilized both ports of Local Memory memory.|__Key__ __Concepts__<br> - Kernel Optimization<br> - 2port BRAM Utilization<br> - two read/write Local Memory<br>__Keywords__<br> - #pragma HLS UNROLL FACTOR=2
[kernel_opt/lmem_2rw_ocl/][]|This is simple example of vector addition to demonstrate how to utilized both ports of Local Memory.|__Key__ __Concepts__<br> - Kernel Optimization<br> - 2port BRAM Utilization<br> - two read/write Local Memory<br>__Keywords__<br> - opencl_unroll_hint(2)
[kernel_opt/loop_fusion_c/][]|This example will demonstrate how to fuse two loops into one to improve the performance of an OpenCL  C/C++ Kernel.|__Key__ __Concepts__<br> - Kernel Optimization<br> - Loop Fusion<br> - Loop Pipelining<br>__Keywords__<br> - #pragma HLS PIPELINE
[kernel_opt/loop_fusion_ocl/][]|This example will demonstrate how to fuse two loops into one to improve the performance of an OpenCL kernel.|__Key__ __Concepts__<br> - Kernel Optimization<br> - Loop Fusion<br> - Loop Pipelining<br>__Keywords__<br> - xcl_pipeline_loop
[kernel_opt/loop_pipeline_ocl/][]|This example demonstrates how loop pipelining can be used to improve the performance of a kernel.|__Key__ __Concepts__<br> - Kernel Optimization<br> - Loop Pipelining<br>__Keywords__<br> - xcl_pipeline_loop
[kernel_opt/loop_reorder_c/][]|This is a simple example of matrix multiplication (Row x Col) to demonstrate how to achieve better pipeline II factor by loop reordering.|__Key__ __Concepts__<br> - Kernel Optimization<br> - Loop reorder to improve II<br>__Keywords__<br> - #pragma HLS PIPELINE<br> - #pragma HLS ARRAY_PARTITION
[kernel_opt/loop_reorder_ocl/][]|This is a simple example of matrix multiplication (Row x Col) to demonstrate how to achieve better pipeline II factor by loop reordering.|__Key__ __Concepts__<br> - Kernel Optimization<br> - Loop reorder to improve II<br>__Keywords__<br> - xcl_pipeline_loop<br> - xcl_array_partition(complete, 2)
[kernel_opt/partition_cyclicblock_c/][]|This example shows how to use array block and cyclic partitioning to improve performance of a kernel|__Key__ __Concepts__<br> - Kernel Optimization<br> - Array Partitioning<br> - Block Partition<br> - Cyclic Partition<br>__Keywords__<br> - #pragma HLS ARRAY_PARTITION<br> - cyclic<br> - block<br> - factor<br> - dim
[kernel_opt/partition_cyclicblock_ocl/][]|This example shows how to use array block and cyclic partitioning to improve performance of a kernel|__Key__ __Concepts__<br> - Kernel Optimization<br> - Array Partitioning<br> - Block Partition<br> - Cyclic Partition<br>__Keywords__<br> - xcl_array_partition<br> - cyclic<br> - block
[kernel_opt/shift_register_c/][]|This example demonstrates how to shift values in registers in each clock cycle|__Key__ __Concepts__<br> - Kernel Optimization<br> - Shift Register<br> - FIR<br>__Keywords__<br> - #pragma HLS ARRAY_PARTITION
[kernel_opt/shift_register_ocl/][]|This example demonstrates how to shift values in registers in each clock cycle|__Key__ __Concepts__<br> - Kernel Optimization<br> - Shift Register<br> - FIR<br>__Keywords__<br> - xcl_array_partition<br> - getprofilingInfo()
[kernel_opt/sum_scan_ocl/][]|This is a simple example to explain the usage of pipeline and array partitioning for designing parallel prefix sum |__Key__ __Concepts__<br> - Kernel Optimization<br> - Array Partitioning<br> - Pipeline<br>__Keywords__<br> - xcl_array_partition<br> - xcl_pipeline_loop
[kernel_opt/systolic_array_c/][]|This is a simple example of matrix multiplication (Row x Col) to help developers learn systolic array based algorithm design. Note : Systolic array based algorithm design is well suited for FPGA.|
[kernel_opt/systolic_array_ocl/][]|This is a simple example of matrix multiplication (Row x Col) to help developers learn systolic array based algorithm design. Note: Systolic array based algorithm design is well suited for FPGA.|
[kernel_opt/vectorization_memorycoalescing_ocl/][]|This example is a simple OpenCL application which highlights the vectorization concept. It provides a basis for calculating the bandwidth utilization when the compiler looking to vectorize.|__Key__ __Concepts__<br> - Vectorization<br> - Memory Coalescing<br>__Keywords__<br> - vec_type_hint
[dataflow/dataflow_func_ocl/][]|This is simple example of vector addition to demonstrate Dataflow functionality in OpenCL Kernel. OpenCL Dataflow allows user to run multiple functions together to achieve higher throughput.|__Key__ __Concepts__<br> - Function/Task Level Parallelism<br>__Keywords__<br> - xcl_dataflow<br> - xclDataflowFifoDepth
[dataflow/dataflow_pipes_ocl/][]|This is simple example of vector addition to demonstrate OpenCL Pipe Memory usage. OpenCL PIPE memory functionality allows user to achieve kernel-to-kernel data transfer without using global memory.|__Key__ __Concepts__<br> - Dataflow<br> - kernel to kernel pipes<br>__Keywords__<br> - pipe<br> - xcl_reqd_pipe_depth<br> - read_pipe_block()<br> - write_pipe_block()
[dataflow/dataflow_stream_array_c/][]|This is simple example of Multiple Stages Vector Addition to demonstrate Array of Stream usage in HLS C Kernel Code.|__Key__ __Concepts__<br> - Array of Stream<br>__Keywords__<br> - dataflow<br> - hls::stream<>
[dataflow/dataflow_stream_c/][]|This is simple example of vector addition to demonstrate Dataflow functionality of HLS. HLS Dataflow allows user to schedule multiple task together to achieve higher throughput.|__Key__ __Concepts__<br> - Task Level Parallelism<br>__Keywords__<br> - dataflow<br> - hls::stream<>
[dataflow/dataflow_subfunc_ocl/][]|This is simple example of vector addition to demonstrate how OpenCL Dataflow allows user to run multiple sub functions together to achieve higher throughput.|__Key__ __Concepts__<br> - SubFunction Level Parallelism<br>__Keywords__<br> - xcl_dataflow<br> - xclDataflowFifoDepth
[clk_freq/critical_path_ocl/][]|This example shows a normal coding style which could lead to critical path issue and design will give degraded timing.  Example also contains better coding style which can improve design timing.|__Key__ __Concepts__<br> - Critical Path handling<br> - Improve Timing<br>
[clk_freq/large_loop_c/][]|This is a CNN (Convolutional Neural Network) based example which mainly focuses on Convolution operation of a CNN network. The goal of this example is to demonstrate a method to overcome kernel design timing failure issue. It also presents the effectiveness of using multiple compute units to improve performance.|__Key__ __Concepts__<br> - Clock Frequency<br> - Multiple Compute Units<br> - Convolutional Neural Networks<br>__Keywords__<br> - #pragma HLS ARRAY_PARTITION<br> - #pragma HLS PIPELINE<br> - #pragma HLS INLINE
[clk_freq/large_loop_ocl/][]|This is a CNN (Convolutional Neural Network) based example which mainly focuses on Convolution operation of a CNN network. The goal of this example is to demonstrate a method to overcome kernel design timing failure issue. It also presents the effectiveness of using multiple compute units to improve performance.|__Key__ __Concepts__<br> - Clock Frequency<br> - Multiple Compute Units<br> - Convolutional Neural Networks<br>__Keywords__<br> - xcl_array_partition<br> - xcl_pipeline_loop<br> - always_inline
[clk_freq/split_kernel_c/][]|This is a multi-filter image processing application to showcase effectiveness of Dataflow/Streams usage. This examples is intended to help developers to break down the complex kernels into multiple sub-functions using HLS Dataflow/Streams. It presents a way to concurrently execute multiple functions with better area utilization compared to a complex single kernel implementation. The main objective of this example is to showcase a way to build a optimal FPGA design which achieves maximum frequency with optimal resource utilization and achieves better performance compared to single complex kernel implementations.|__Key__ __Concepts__<br> - Dataflow<br> - Stream<br>__Keywords__<br> - #pragma HLS DATAFLOW<br> - hls::stream<br> - #pragma HLS INLINE<br> - #pragma HLS ARRAY_PARTITION<br> - #pragma HLS PIPELINE
[clk_freq/split_kernel_ocl/][]|This is a multi-filter image processing application to showcase effectiveness of Dataflow/Streams usage. This examples is intended to help developers to break down the complex kernel into multiple sub-functions using OpenCL Dataflow. It presents a way to concurrently execute multiple functions with better area utilization compared to a complex single kernel implementation. The main objective of this example is to showcase a way to build a optimal FPGA design which achieves maximum frequency with optimal resource utilization and achieves better performance compared to single kernel implementations.|__Key__ __Concepts__<br> - Dataflow<br> - Stream<br>__Keywords__<br> - xcl_dataflow<br> - xcl_array_partition<br> - xcl_pipeline_loop
[clk_freq/too_many_cu_c/][]|This is simple example of vector addition to demonstrate effectiveness of using single compute unit with heavy work load to achieve better performance. Bad example uses multiple compute units to achieve good performance but it results in heavy usage of FPGA resources and area due to which design fails timing. Good example uses single compute unit to compute with heavier work load, it helps in less resource utilization and also helps in kernel scalability. To switch between Good/Bad cases use the flag provided in makefile.|__Key__ __Concepts__<br> - Clock Frequency<br> - Data Level Parallelism<br> - Multiple Compute Units<br>__Keywords__<br> - #pragma HLS PIPELINE<br> - #pragma HLS ARRAY_PARTITION
[clk_freq/too_many_cu_ocl/][]|This is simple example of vector addition to demonstrate effectiveness of using single compute unit with heavy work load to achieve better performance. Bad example uses multiple compute units to achieve good performance but it results in heavy usage of FPGA resources and area due to which design fails timing. Good example uses single compute unit to compute with heavier work load, it helps in less resource utilization and also helps in kernel scalability. To switch between Good/Bad cases use the flag provided in makefile.|__Key__ __Concepts__<br> - Clock Frequency<br> - Data Level Parallelism<br> - Multiple Compute Units<br>__Keywords__<br> - xcl_array_partition(complete, 1)<br> - xcl_pipeline_loop
[debug/debug_printf_ocl/][]|This is simple example of vector addition and printing of data that is computational result (addition). It is based on vectored addition that demonstrates printing of work item data (integer product in this case)|__Key__ __Concepts__<br> - Use of print statements for debugging<br>__Keywords__<br> - printf<br> - param:compiler.enableAutoPipelining=false
[debug/debug_profile_ocl/][]|This is simple example of vector addition and printing profile data (wall clock time taken between start and stop). It also dump a waveform file which can be reloaded to vivado to see the waveform. Run command 'vivado -source ./scripts/open_waveform.tcl -tclargs <device_name>-<kernel_name>.<target>.<device_name>.wdb' to launch waveform viewer. User can also update batch to gui in sdaccel.ini file to see the live waveform while running application.|__Key__ __Concepts__<br> - Use of Profile API<br> - Waveform Dumping and loading<br>
[rtl_kernel/rtl_adder_pipes/][]|This example shows an adder with pipes using 3 RTL kernels.|__Key__ __Concepts__<br> - RTL Kernel<br> - Multiple RTL Kernels<br>
[rtl_kernel/rtl_vadd/][]|Simple example of vector addition using RTL Kernel|__Key__ __Concepts__<br> - RTL Kernel<br>
[rtl_kernel/rtl_vadd_2clks/][]|This example shows vector addition with 2 kernel clocks using RTL Kernel.|__Key__ __Concepts__<br> - RTL Kernel<br> - Multiple Kernel Clocks<br>__Keywords__<br> - --kernel_frequency
[rtl_kernel/rtl_vadd_2kernels/][]|This example has two RTL Kernels. Both Kernel_0 and Kernel_1 perform vector addition. The Kernel_1 reads the output from Kernel_0 as one of two inputs.|__Key__ __Concepts__<br> - Multiple RTL Kernels<br>
[rtl_kernel/rtl_vadd_hw_debug/][]|This is an example that showcases the Hardware Debug of Vector Addition RTL Kernel in Hardware.|__Key__ __Concepts__<br> - RTL Kernel Debug<br>
[rtl_kernel/rtl_vadd_mixed_c_vadd/][]|This example has one RTL kernel and one C kernel. Both RTL kernel and C kernel perform vector addition. The C kernel reads the output from RTL kernel as one of two inputs.|__Key__ __Concepts__<br> - Mixed Kernels<br>

[hello_world]:hello_world
[hello_world/helloworld_c/]:hello_world/helloworld_c/
[hello_world/helloworld_ocl/]:hello_world/helloworld_ocl/
[cpu_to_fpga]:cpu_to_fpga
[cpu_to_fpga/00_cpu/]:cpu_to_fpga/00_cpu/
[cpu_to_fpga/01_ocl/]:cpu_to_fpga/01_ocl/
[cpu_to_fpga/02_lmem_ocl/]:cpu_to_fpga/02_lmem_ocl/
[cpu_to_fpga/03_burst_rw_ocl/]:cpu_to_fpga/03_burst_rw_ocl/
[cpu_to_fpga/04_partition_ocl/]:cpu_to_fpga/04_partition_ocl/
[host]:host
[host/concurrent_kernel_execution_c/]:host/concurrent_kernel_execution_c/
[host/copy_buffer_c/]:host/copy_buffer_c/
[host/data_transfer_c/]:host/data_transfer_c/
[host/device_query_c/]:host/device_query_c/
[host/device_query_cpp/]:host/device_query_cpp/
[host/errors_c/]:host/errors_c/
[host/errors_cpp/]:host/errors_cpp/
[host/host_global_bandwidth/]:host/host_global_bandwidth/
[host/kernel_swap_c/]:host/kernel_swap_c/
[host/multiple_devices_c/]:host/multiple_devices_c/
[host/multiple_process_c/]:host/multiple_process_c/
[host/overlap_c/]:host/overlap_c/
[host/stream_access_c/]:host/stream_access_c/
[host/sub_devices_c/]:host/sub_devices_c/
[kernel_to_gmem]:kernel_to_gmem
[kernel_to_gmem/burst_rw_c/]:kernel_to_gmem/burst_rw_c/
[kernel_to_gmem/burst_rw_ocl/]:kernel_to_gmem/burst_rw_ocl/
[kernel_to_gmem/custom_datatype_c/]:kernel_to_gmem/custom_datatype_c/
[kernel_to_gmem/custom_datatype_ocl/]:kernel_to_gmem/custom_datatype_ocl/
[kernel_to_gmem/full_array_2d_c/]:kernel_to_gmem/full_array_2d_c/
[kernel_to_gmem/full_array_2d_ocl/]:kernel_to_gmem/full_array_2d_ocl/
[kernel_to_gmem/gmem_2banks_c/]:kernel_to_gmem/gmem_2banks_c/
[kernel_to_gmem/gmem_2banks_ocl/]:kernel_to_gmem/gmem_2banks_ocl/
[kernel_to_gmem/kernel_global_bandwidth/]:kernel_to_gmem/kernel_global_bandwidth/
[kernel_to_gmem/memcoalesce_hang_c/]:kernel_to_gmem/memcoalesce_hang_c/
[kernel_to_gmem/plram_access_c/]:kernel_to_gmem/plram_access_c/
[kernel_to_gmem/row_array_2d_c/]:kernel_to_gmem/row_array_2d_c/
[kernel_to_gmem/row_array_2d_ocl/]:kernel_to_gmem/row_array_2d_ocl/
[kernel_to_gmem/slr_assign/]:kernel_to_gmem/slr_assign/
[kernel_to_gmem/wide_mem_rw_c/]:kernel_to_gmem/wide_mem_rw_c/
[kernel_to_gmem/wide_mem_rw_ocl/]:kernel_to_gmem/wide_mem_rw_ocl/
[kernel_to_gmem/window_array_2d_c/]:kernel_to_gmem/window_array_2d_c/
[kernel_to_gmem/window_array_2d_ocl/]:kernel_to_gmem/window_array_2d_ocl/
[kernel_opt]:kernel_opt
[kernel_opt/aos_vs_soa_ocl/]:kernel_opt/aos_vs_soa_ocl/
[kernel_opt/array_partition_c/]:kernel_opt/array_partition_c/
[kernel_opt/array_partition_ocl/]:kernel_opt/array_partition_ocl/
[kernel_opt/dependence_inter_c/]:kernel_opt/dependence_inter_c/
[kernel_opt/lmem_2rw_c/]:kernel_opt/lmem_2rw_c/
[kernel_opt/lmem_2rw_ocl/]:kernel_opt/lmem_2rw_ocl/
[kernel_opt/loop_fusion_c/]:kernel_opt/loop_fusion_c/
[kernel_opt/loop_fusion_ocl/]:kernel_opt/loop_fusion_ocl/
[kernel_opt/loop_pipeline_ocl/]:kernel_opt/loop_pipeline_ocl/
[kernel_opt/loop_reorder_c/]:kernel_opt/loop_reorder_c/
[kernel_opt/loop_reorder_ocl/]:kernel_opt/loop_reorder_ocl/
[kernel_opt/partition_cyclicblock_c/]:kernel_opt/partition_cyclicblock_c/
[kernel_opt/partition_cyclicblock_ocl/]:kernel_opt/partition_cyclicblock_ocl/
[kernel_opt/shift_register_c/]:kernel_opt/shift_register_c/
[kernel_opt/shift_register_ocl/]:kernel_opt/shift_register_ocl/
[kernel_opt/sum_scan_ocl/]:kernel_opt/sum_scan_ocl/
[kernel_opt/systolic_array_c/]:kernel_opt/systolic_array_c/
[kernel_opt/systolic_array_ocl/]:kernel_opt/systolic_array_ocl/
[kernel_opt/vectorization_memorycoalescing_ocl/]:kernel_opt/vectorization_memorycoalescing_ocl/
[dataflow]:dataflow
[dataflow/dataflow_func_ocl/]:dataflow/dataflow_func_ocl/
[dataflow/dataflow_pipes_ocl/]:dataflow/dataflow_pipes_ocl/
[dataflow/dataflow_stream_array_c/]:dataflow/dataflow_stream_array_c/
[dataflow/dataflow_stream_c/]:dataflow/dataflow_stream_c/
[dataflow/dataflow_subfunc_ocl/]:dataflow/dataflow_subfunc_ocl/
[clk_freq]:clk_freq
[clk_freq/critical_path_ocl/]:clk_freq/critical_path_ocl/
[clk_freq/large_loop_c/]:clk_freq/large_loop_c/
[clk_freq/large_loop_ocl/]:clk_freq/large_loop_ocl/
[clk_freq/split_kernel_c/]:clk_freq/split_kernel_c/
[clk_freq/split_kernel_ocl/]:clk_freq/split_kernel_ocl/
[clk_freq/too_many_cu_c/]:clk_freq/too_many_cu_c/
[clk_freq/too_many_cu_ocl/]:clk_freq/too_many_cu_ocl/
[debug]:debug
[debug/debug_printf_ocl/]:debug/debug_printf_ocl/
[debug/debug_profile_ocl/]:debug/debug_profile_ocl/
[rtl_kernel]:rtl_kernel
[rtl_kernel/rtl_adder_pipes/]:rtl_kernel/rtl_adder_pipes/
[rtl_kernel/rtl_vadd/]:rtl_kernel/rtl_vadd/
[rtl_kernel/rtl_vadd_2clks/]:rtl_kernel/rtl_vadd_2clks/
[rtl_kernel/rtl_vadd_2kernels/]:rtl_kernel/rtl_vadd_2kernels/
[rtl_kernel/rtl_vadd_hw_debug/]:rtl_kernel/rtl_vadd_hw_debug/
[rtl_kernel/rtl_vadd_mixed_c_vadd/]:rtl_kernel/rtl_vadd_mixed_c_vadd/
