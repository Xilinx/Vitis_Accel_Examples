Kernel Optimization Examples
==================================
Kernel Optimization for performance

 __Examples Table__ 

Example        | Description           | Key Concepts / Keywords 
---------------|-----------------------|-------------------------
[aos_vs_soa_ocl/][]|This example demonstrates how data layout can impact the performance of certain kernels. The example we will demonstrate how using the Structure of Array data layout can impact certain data parallel problems.|__Key__ __Concepts__<br> - Kernel Optimization<br> - Data Layout<br>
[array_partition_c/][]|This is a simple example of matrix multiplication (Row x Col) to demonstrate how to achieve better performance by array partitioning, using HLS kernel in SDx Environment.|__Key__ __Concepts__<br> - Kernel Optimization<br> - HLS C Kernel<br> - Array Partition<br>__Keywords__<br> - #pragma HLS ARRAY_PARTITION<br> - complete
[array_partition_ocl/][]|This example shows how to use array partitioning to improve performance of a kernel|__Key__ __Concepts__<br> - Kernel Optimization<br> - Array Partitioning<br>__Keywords__<br> - xcl_array_partition<br> - complete
[dependence_inter_c/][]|This Example demonstrates the HLS pragma 'DEPENDENCE'.Using 'DEPENDENCE' pragma, user can provide additional dependency details to the compiler by specifying if the dependency in consecutive loop iterations on buffer is true/false, which allows the compiler to perform unrolling/pipelining to get better performance.|__Key__ __Concepts__<br> - Inter Dependence<br>__Keywords__<br> - DEPENDENCE<br> - inter<br> - WAR
[lmem_2rw_c/][]|This is simple example of vector addition to demonstrate how to utilized both ports of Local Memory memory.|__Key__ __Concepts__<br> - Kernel Optimization<br> - 2port BRAM Utilization<br> - two read/write Local Memory<br>__Keywords__<br> - #pragma HLS UNROLL FACTOR=2
[lmem_2rw_ocl/][]|This is simple example of vector addition to demonstrate how to utilized both ports of Local Memory.|__Key__ __Concepts__<br> - Kernel Optimization<br> - 2port BRAM Utilization<br> - two read/write Local Memory<br>__Keywords__<br> - opencl_unroll_hint(2)
[loop_fusion_c/][]|This example will demonstrate how to fuse two loops into one to improve the performance of an OpenCL  C/C++ Kernel.|__Key__ __Concepts__<br> - Kernel Optimization<br> - Loop Fusion<br> - Loop Pipelining<br>__Keywords__<br> - #pragma HLS PIPELINE
[loop_fusion_ocl/][]|This example will demonstrate how to fuse two loops into one to improve the performance of an OpenCL kernel.|__Key__ __Concepts__<br> - Kernel Optimization<br> - Loop Fusion<br> - Loop Pipelining<br>__Keywords__<br> - xcl_pipeline_loop
[loop_pipeline_ocl/][]|This example demonstrates how loop pipelining can be used to improve the performance of a kernel.|__Key__ __Concepts__<br> - Kernel Optimization<br> - Loop Pipelining<br>__Keywords__<br> - xcl_pipeline_loop
[loop_reorder_c/][]|This is a simple example of matrix multiplication (Row x Col) to demonstrate how to achieve better pipeline II factor by loop reordering.|__Key__ __Concepts__<br> - Kernel Optimization<br> - Loop reorder to improve II<br>__Keywords__<br> - #pragma HLS PIPELINE<br> - #pragma HLS ARRAY_PARTITION
[loop_reorder_ocl/][]|This is a simple example of matrix multiplication (Row x Col) to demonstrate how to achieve better pipeline II factor by loop reordering.|__Key__ __Concepts__<br> - Kernel Optimization<br> - Loop reorder to improve II<br>__Keywords__<br> - xcl_pipeline_loop<br> - xcl_array_partition(complete, 2)
[partition_cyclicblock_c/][]|This example shows how to use array block and cyclic partitioning to improve performance of a kernel|__Key__ __Concepts__<br> - Kernel Optimization<br> - Array Partitioning<br> - Block Partition<br> - Cyclic Partition<br>__Keywords__<br> - #pragma HLS ARRAY_PARTITION<br> - cyclic<br> - block<br> - factor<br> - dim
[partition_cyclicblock_ocl/][]|This example shows how to use array block and cyclic partitioning to improve performance of a kernel|__Key__ __Concepts__<br> - Kernel Optimization<br> - Array Partitioning<br> - Block Partition<br> - Cyclic Partition<br>__Keywords__<br> - xcl_array_partition<br> - cyclic<br> - block
[shift_register_c/][]|This example demonstrates how to shift values in registers in each clock cycle|__Key__ __Concepts__<br> - Kernel Optimization<br> - Shift Register<br> - FIR<br>__Keywords__<br> - #pragma HLS ARRAY_PARTITION
[shift_register_ocl/][]|This example demonstrates how to shift values in registers in each clock cycle|__Key__ __Concepts__<br> - Kernel Optimization<br> - Shift Register<br> - FIR<br>__Keywords__<br> - xcl_array_partition<br> - getprofilingInfo()
[sum_scan_ocl/][]|This is a simple example to explain the usage of pipeline and array partitioning for designing parallel prefix sum |__Key__ __Concepts__<br> - Kernel Optimization<br> - Array Partitioning<br> - Pipeline<br>__Keywords__<br> - xcl_array_partition<br> - xcl_pipeline_loop
[systolic_array_c/][]|This is a simple example of matrix multiplication (Row x Col) to help developers learn systolic array based algorithm design. Note : Systolic array based algorithm design is well suited for FPGA.|
[systolic_array_ocl/][]|This is a simple example of matrix multiplication (Row x Col) to help developers learn systolic array based algorithm design. Note: Systolic array based algorithm design is well suited for FPGA.|
[vectorization_memorycoalescing_ocl/][]|This example is a simple OpenCL application which highlights the vectorization concept. It provides a basis for calculating the bandwidth utilization when the compiler looking to vectorize.|__Key__ __Concepts__<br> - Vectorization<br> - Memory Coalescing<br>__Keywords__<br> - vec_type_hint

[.]:.
[aos_vs_soa_ocl/]:aos_vs_soa_ocl/
[array_partition_c/]:array_partition_c/
[array_partition_ocl/]:array_partition_ocl/
[dependence_inter_c/]:dependence_inter_c/
[lmem_2rw_c/]:lmem_2rw_c/
[lmem_2rw_ocl/]:lmem_2rw_ocl/
[loop_fusion_c/]:loop_fusion_c/
[loop_fusion_ocl/]:loop_fusion_ocl/
[loop_pipeline_ocl/]:loop_pipeline_ocl/
[loop_reorder_c/]:loop_reorder_c/
[loop_reorder_ocl/]:loop_reorder_ocl/
[partition_cyclicblock_c/]:partition_cyclicblock_c/
[partition_cyclicblock_ocl/]:partition_cyclicblock_ocl/
[shift_register_c/]:shift_register_c/
[shift_register_ocl/]:shift_register_ocl/
[sum_scan_ocl/]:sum_scan_ocl/
[systolic_array_c/]:systolic_array_c/
[systolic_array_ocl/]:systolic_array_ocl/
[vectorization_memorycoalescing_ocl/]:vectorization_memorycoalescing_ocl/
