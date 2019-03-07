Dataflow Examples
==================================
Kernel Optimization through Macro Level Pipelining

 __Examples Table__ 

Example        | Description           | Key Concepts / Keywords 
---------------|-----------------------|-------------------------
[dataflow_func_ocl/][]|This is simple example of vector addition to demonstrate Dataflow functionality in OpenCL Kernel. OpenCL Dataflow allows user to run multiple functions together to achieve higher throughput.|__Key__ __Concepts__<br> - Function/Task Level Parallelism<br>__Keywords__<br> - xcl_dataflow<br> - xclDataflowFifoDepth
[dataflow_pipes_ocl/][]|This is simple example of vector addition to demonstrate OpenCL Pipe Memory usage. OpenCL PIPE memory functionality allows user to achieve kernel-to-kernel data transfer without using global memory.|__Key__ __Concepts__<br> - Dataflow<br> - kernel to kernel pipes<br>__Keywords__<br> - pipe<br> - xcl_reqd_pipe_depth<br> - read_pipe_block()<br> - write_pipe_block()
[dataflow_stream_array_c/][]|This is simple example of Multiple Stages Vector Addition to demonstrate Array of Stream usage in HLS C Kernel Code.|__Key__ __Concepts__<br> - Array of Stream<br>__Keywords__<br> - dataflow<br> - hls::stream<>
[dataflow_stream_c/][]|This is simple example of vector addition to demonstrate Dataflow functionality of HLS. HLS Dataflow allows user to schedule multiple task together to achieve higher throughput.|__Key__ __Concepts__<br> - Task Level Parallelism<br>__Keywords__<br> - dataflow<br> - hls::stream<>
[dataflow_subfunc_ocl/][]|This is simple example of vector addition to demonstrate how OpenCL Dataflow allows user to run multiple sub functions together to achieve higher throughput.|__Key__ __Concepts__<br> - SubFunction Level Parallelism<br>__Keywords__<br> - xcl_dataflow<br> - xclDataflowFifoDepth

[.]:.
[dataflow_func_ocl/]:dataflow_func_ocl/
[dataflow_pipes_ocl/]:dataflow_pipes_ocl/
[dataflow_stream_array_c/]:dataflow_stream_array_c/
[dataflow_stream_c/]:dataflow_stream_c/
[dataflow_subfunc_ocl/]:dataflow_subfunc_ocl/
