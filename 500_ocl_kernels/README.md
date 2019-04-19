OpenCL Kernel Examples
==================================
This page contains examples for users who are new to Xilinx SDx OpenCL Flows. The focus of the examples is towards code optimization using OpenCL kernels for Xilinx devices.

 __Examples Table__ 

Example        | Description           | Key Concepts / Keywords 
---------------|-----------------------|-------------------------
[501_cl_large_loop/][]|This is a CNN (Convolutional Neural Network) based example which mainly focuses on Convolution operation of a CNN network. The goal of this example is to demonstrate a method to overcome kernel design timing failure issue. It also presents the effectiveness of using multiple compute units to improve performance.|__Key__ __Concepts__<br> - Clock Frequency<br> - Multiple Compute Units<br> - Convolutional Neural Networks<br>__Keywords__<br> - xcl_array_partition<br> - xcl_pipeline_loop<br> - always_inline
[502_cl_split_kernel/][]|This is a multi-filter image processing application to showcase effectiveness of Dataflow/Streams usage. This examples is intended to help developers to break down the complex kernel into multiple sub-functions using OpenCL Dataflow. It presents a way to concurrently execute multiple functions with better area utilization compared to a complex single kernel implementation. The main objective of this example is to showcase a way to build a optimal FPGA design which achieves maximum frequency with optimal resource utilization and achieves better performance compared to single kernel implementations.|__Key__ __Concepts__<br> - Dataflow<br> - Stream<br>__Keywords__<br> - xcl_dataflow<br> - xcl_array_partition<br> - xcl_pipeline_loop
[503_cl_too_many_cu/][]|This is simple example of vector addition to demonstrate effectiveness of using single compute unit with heavy work load to achieve better performance. Bad example uses multiple compute units to achieve good performance but it results in heavy usage of FPGA resources and area due to which design fails timing. Good example uses single compute unit to compute with heavier work load, it helps in less resource utilization and also helps in kernel scalability. To switch between Good/Bad cases use the flag provided in makefile.|__Key__ __Concepts__<br> - Clock Frequency<br> - Data Level Parallelism<br> - Multiple Compute Units<br>__Keywords__<br> - xcl_array_partition(complete, 1)<br> - xcl_pipeline_loop
[504_cl_dataflow_func/][]|This is simple example of vector addition to demonstrate Dataflow functionality in OpenCL Kernel. OpenCL Dataflow allows user to run multiple functions together to achieve higher throughput.|__Key__ __Concepts__<br> - Function/Task Level Parallelism<br>__Keywords__<br> - xcl_dataflow<br> - xclDataflowFifoDepth
[505_cl_dataflow_pipes/][]|This is simple example of vector addition to demonstrate OpenCL Pipe Memory usage. OpenCL PIPE memory functionality allows user to achieve kernel-to-kernel data transfer without using global memory.|__Key__ __Concepts__<br> - Dataflow<br> - kernel to kernel pipes<br>__Keywords__<br> - pipe<br> - xcl_reqd_pipe_depth<br> - read_pipe_block()<br> - write_pipe_block()
[506_cl_dataflow_subfunc/][]|This is simple example of vector addition to demonstrate how OpenCL Dataflow allows user to run multiple sub functions together to achieve higher throughput.|__Key__ __Concepts__<br> - SubFunction Level Parallelism<br>__Keywords__<br> - xcl_dataflow<br> - xclDataflowFifoDepth
[507_cl_debug_printf/][]|This is simple example of vector addition and printing of data that is computational result (addition). It is based on vectored addition that demonstrates printing of work item data (integer product in this case)|__Key__ __Concepts__<br> - Use of print statements for debugging<br>__Keywords__<br> - printf<br> - param:compiler.enableAutoPipelining=false
[508_cl_debug_profile/][]|This is simple example of vector addition and printing profile data (wall clock time taken between start and stop). It also dump a waveform file which can be reloaded to vivado to see the waveform. Run command 'vivado -source ./scripts/open_waveform.tcl -tclargs <device_name>-<kernel_name>.<target>.<device_name>.wdb' to launch waveform viewer. User can also update batch to gui in sdaccel.ini file to see the live waveform while running application.|__Key__ __Concepts__<br> - Use of Profile API<br> - Waveform Dumping and loading<br>
[509_cl_helloworld/][]|This example is a simple OpenCL application. It will highlight the basic flow of an OpenCL application.|__Key__ __Concepts__<br> - OpenCL API<br>
[510_cl_array_partition/][]|This example shows how to use array partitioning to improve performance of a kernel|__Key__ __Concepts__<br> - Kernel Optimization<br> - Array Partitioning<br>__Keywords__<br> - xcl_array_partition<br> - complete
[511_cl_lmem_2rw/][]|This is simple example of vector addition to demonstrate how to utilized both ports of Local Memory.|__Key__ __Concepts__<br> - Kernel Optimization<br> - 2port BRAM Utilization<br> - two read/write Local Memory<br>__Keywords__<br> - opencl_unroll_hint(2)
[512_cl_loop_fusion/][]|This example will demonstrate how to fuse two loops into one to improve the performance of an OpenCL kernel.|__Key__ __Concepts__<br> - Kernel Optimization<br> - Loop Fusion<br> - Loop Pipelining<br>__Keywords__<br> - xcl_pipeline_loop
[513_cl_loop_reorder/][]|This is a simple example of matrix multiplication (Row x Col) to demonstrate how to achieve better pipeline II factor by loop reordering.|__Key__ __Concepts__<br> - Kernel Optimization<br> - Loop reorder to improve II<br>__Keywords__<br> - xcl_pipeline_loop<br> - xcl_array_partition(complete, 2)
[514_cl_partition_cyclicblock/][]|This example shows how to use array block and cyclic partitioning to improve performance of a kernel|__Key__ __Concepts__<br> - Kernel Optimization<br> - Array Partitioning<br> - Block Partition<br> - Cyclic Partition<br>__Keywords__<br> - xcl_array_partition<br> - cyclic<br> - block
[515_cl_shift_register/][]|This example demonstrates how to shift values in registers in each clock cycle|__Key__ __Concepts__<br> - Kernel Optimization<br> - Shift Register<br> - FIR<br>__Keywords__<br> - xcl_array_partition<br> - getprofilingInfo()
[516_cl_sum_scan/][]|This is a simple example to explain the usage of pipeline and array partitioning for designing parallel prefix sum |__Key__ __Concepts__<br> - Kernel Optimization<br> - Array Partitioning<br> - Pipeline<br>__Keywords__<br> - xcl_array_partition<br> - xcl_pipeline_loop
[517_cl_systolic_array/][]|This is a simple example of matrix multiplication (Row x Col) to help developers learn systolic array based algorithm design. Note: Systolic array based algorithm design is well suited for FPGA.|
[518_cl_vectorization_memorycoalescing/][]|This example is a simple OpenCL application which highlights the vectorization concept. It provides a basis for calculating the bandwidth utilization when the compiler looking to vectorize.|__Key__ __Concepts__<br> - Vectorization<br> - Memory Coalescing<br>__Keywords__<br> - vec_type_hint
[519_cl_burst_rw/][]|This is simple example of using AXI4-master interface for burst read and write|__Key__ __Concepts__<br> - burst access<br>__Keywords__<br> - param:compiler.interfaceWrBurstLen<br> - param:compiler.interfaceRdBurstLen
[520_cl_full_array_2d/][]|This is a simple example of accessing full data from 2d array|__Key__ __Concepts__<br> - 2D data full array Access<br>
[521_cl_gmem_2banks/][]|This example of 2ddr to demonstrate on how to use 2ddr DSA. How to create buffers in each DDR.|__Key__ __Concepts__<br> - Multiple Banks<br>__Keywords__<br> - max_memory_ports<br> - misc:map_connect<br> - XCL_MEM_DDR_BANK0<br> - XCL_MEM_DDR_BANK1<br> - XCL_MEM_DDR_BANKx
[522_cl_row_array_2d/][]|This is a simple example of accessing each row of data from 2d array|__Key__ __Concepts__<br> - Row of 2D data array access<br>__Keywords__<br> - xcl_dataflow<br> - xcl_pipeline_loop
[523_cl_wide_mem_rw/][]|This is simple example of vector addition to demonstrate Wide Memory Access using uint16 data type. Based on input argument type, xocc compiler will figure our the memory datawidth between Global Memory and Kernel. For this example, uint16 datatype is used, so Memory datawidth will be 16 x (integer bit size) = 16 x 32 = 512 bit.|__Key__ __Concepts__<br> - Kernel to DDR<br> - wide memory access<br> - burst read and write<br>__Keywords__<br> - uint16<br> - xcl_pipeline_loop

[.]:.
[501_cl_large_loop/]:501_cl_large_loop/
[502_cl_split_kernel/]:502_cl_split_kernel/
[503_cl_too_many_cu/]:503_cl_too_many_cu/
[504_cl_dataflow_func/]:504_cl_dataflow_func/
[505_cl_dataflow_pipes/]:505_cl_dataflow_pipes/
[506_cl_dataflow_subfunc/]:506_cl_dataflow_subfunc/
[507_cl_debug_printf/]:507_cl_debug_printf/
[508_cl_debug_profile/]:508_cl_debug_profile/
[509_cl_helloworld/]:509_cl_helloworld/
[510_cl_array_partition/]:510_cl_array_partition/
[511_cl_lmem_2rw/]:511_cl_lmem_2rw/
[512_cl_loop_fusion/]:512_cl_loop_fusion/
[513_cl_loop_reorder/]:513_cl_loop_reorder/
[514_cl_partition_cyclicblock/]:514_cl_partition_cyclicblock/
[515_cl_shift_register/]:515_cl_shift_register/
[516_cl_sum_scan/]:516_cl_sum_scan/
[517_cl_systolic_array/]:517_cl_systolic_array/
[518_cl_vectorization_memorycoalescing/]:518_cl_vectorization_memorycoalescing/
[519_cl_burst_rw/]:519_cl_burst_rw/
[520_cl_full_array_2d/]:520_cl_full_array_2d/
[521_cl_gmem_2banks/]:521_cl_gmem_2banks/
[522_cl_row_array_2d/]:522_cl_row_array_2d/
[523_cl_wide_mem_rw/]:523_cl_wide_mem_rw/
