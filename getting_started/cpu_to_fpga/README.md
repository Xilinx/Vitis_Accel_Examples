CPU to FPGA Examples
==================================
Labs to showcase the cpu to fpga conversion with kernel optimizations.

 __Examples Table__ 

Example        | Description           | Key Concepts / Keywords 
---------------|-----------------------|-------------------------
[00_cpu/][]|This is a simple example of matrix multiplication (Row x Col).|
[01_ocl/][]|This is a simple example of OpenCL matrix multiplication (Row x Col).|__Key__ __Concepts__<br> - OpenCL APIs<br>
[02_lmem_ocl/][]|This is a simple example of matrix multiplication (Row x Col) to demonstrate how to reduce number of memory accesses using local memory.|__Key__ __Concepts__<br> - Kernel Optimization<br> - Local Memory<br>
[03_burst_rw_ocl/][]|This is a simple example of matrix multiplication (Row x Col) to demonstrate how to achieve better pipeline with burst read and write to/from local memory from/to DDR.|__Key__ __Concepts__<br> - Kernel Optimization<br> - Burst Read/Write<br>
[04_partition_ocl/][]|This is a simple example of matrix multiplication (Row x Col) to demonstrate how to achieve better performance by array partitioning and loop unrolling.|__Key__ __Concepts__<br> - Array Partition<br> - Loop Unroll<br>__Keywords__<br> - xcl_pipeline_loop<br> - xcl_array_partition(complete, dim)<br> - opencl_unroll_hint

[.]:.
[00_cpu/]:00_cpu/
[01_ocl/]:01_ocl/
[02_lmem_ocl/]:02_lmem_ocl/
[03_burst_rw_ocl/]:03_burst_rw_ocl/
[04_partition_ocl/]:04_partition_ocl/
