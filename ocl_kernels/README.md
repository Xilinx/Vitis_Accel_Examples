OpenCL Kernel Examples
==================================
This section contains OpenCL Kernel Examples.

 __Examples Table__ 

Example        | Description           | Key Concepts / Keywords 
---------------|-----------------------|-------------------------
[cl_array_partition/][]|This example shows how to use array partitioning to improve performance of a kernel|__Key__ __Concepts__<br> - Kernel Optimization<br> - Array Partitioning<br>__Keywords__<br> - xcl_array_partition<br> - complete
[cl_burst_rw/][]|This is simple example of using AXI4-master interface for burst read and write|__Key__ __Concepts__<br> - burst access<br>__Keywords__<br> - compiler.interfaceRdBurstLen<br> - compiler.interfaceWrBurstLen
[cl_dataflow_func/][]|This is simple example of vector addition to demonstrate Dataflow functionality in OpenCL Kernel. OpenCL Dataflow allows user to run multiple functions together to achieve higher throughput.|__Key__ __Concepts__<br> - Function/Task Level Parallelism<br>__Keywords__<br> - xcl_dataflow<br> - xclDataflowFifoDepth
[cl_dataflow_subfunc/][]|This is simple example of vector addition to demonstrate how OpenCL Dataflow allows user to run multiple sub functions together to achieve higher throughput.|__Key__ __Concepts__<br> - SubFunction Level Parallelism<br>__Keywords__<br> - xcl_dataflow<br> - xclDataflowFifoDepth
[cl_gmem_2banks/][]|This example of 2ddr to demonstrate on how to use 2ddr XSA. How to create buffers in each DDR.|__Key__ __Concepts__<br> - Multiple Banks<br>__Keywords__<br> - sp<br> - connectivity
[cl_helloworld/][]|This example is a simple OpenCL application. It will highlight the basic flow of an OpenCL application.|__Key__ __Concepts__<br> - OpenCL API<br>
[cl_lmem_2rw/][]|This is simple example of vector addition to demonstrate how to utilized both ports of Local Memory.|__Key__ __Concepts__<br> - Kernel Optimization<br> - 2port BRAM Utilization<br> - two read/write Local Memory<br>__Keywords__<br> - opencl_unroll_hint
[cl_loop_reorder/][]|This is a simple example of matrix multiplication (Row x Col) to demonstrate how to achieve better pipeline II factor by loop reordering.|__Key__ __Concepts__<br> - Kernel Optimization<br> - Loop reorder to improve II<br>__Keywords__<br> - xcl_pipeline_loop<br> - xcl_array_partition
[cl_partition_cyclicblock/][]|This example shows how to use array block and cyclic partitioning to improve performance of a kernel|__Key__ __Concepts__<br> - Kernel Optimization<br> - Array Partitioning<br> - Block Partition<br> - Cyclic Partition<br>__Keywords__<br> - xcl_array_partition<br> - cyclic<br> - block
[cl_shift_register/][]|This example demonstrates how to shift values in registers in each clock cycle|__Key__ __Concepts__<br> - Kernel Optimization<br> - Shift Register<br> - FIR<br>__Keywords__<br> - xcl_array_partition<br> - getProfilingInfo
[cl_systolic_array/][]|This is a simple example of matrix multiplication (Row x Col) to help developers learn systolic array based algorithm design. Note: Systolic array based algorithm design is well suited for FPGA.|
[cl_wide_mem_rw/][]|This is simple example of vector addition to demonstrate Wide Memory Access using uint16 data type. Based on input argument type, V++ compiler will figure our the memory datawidth between Global Memory and Kernel. For this example, uint16 datatype is used, so Memory datawidth will be 16 x (integer bit size) = 16 x 32 = 512 bit.|__Key__ __Concepts__<br> - Kernel to DDR<br> - wide memory access<br> - burst read and write<br>__Keywords__<br> - uint16<br> - xcl_pipeline_loop

[.]:.
[cl_array_partition/]:cl_array_partition/
[cl_burst_rw/]:cl_burst_rw/
[cl_dataflow_func/]:cl_dataflow_func/
[cl_dataflow_subfunc/]:cl_dataflow_subfunc/
[cl_gmem_2banks/]:cl_gmem_2banks/
[cl_helloworld/]:cl_helloworld/
[cl_lmem_2rw/]:cl_lmem_2rw/
[cl_loop_reorder/]:cl_loop_reorder/
[cl_partition_cyclicblock/]:cl_partition_cyclicblock/
[cl_shift_register/]:cl_shift_register/
[cl_systolic_array/]:cl_systolic_array/
[cl_wide_mem_rw/]:cl_wide_mem_rw/
