System Compiler Examples
==================================
This section contains System Compiler based Examples.

 __Examples Table__ 

Example        | Description           | Key Concepts / Keywords 
---------------|-----------------------|-------------------------
[debug_profile_sc/][]|This is simple example of matrix multiplication and printing profile data. This design also explains how multile compute units are used in pipelined manner.|__Key__ __Concepts__<br> - Debug<br> - Profiling<br> - Pipelining<br>__Keywords__<br> - PROFILE_KERNEL<br> - PROFILE_PORT<br> - PROFILE_OFFLOAD<br> - [pragma HLS PIPELINE](https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-pipeline)<br> - pragma HLS UNROLL<br> - vpp::stream<>
[quick_start_sc/][]|This is quick start example based on a video filtering usecase to describe system compiler usage in Vitis Environment.|__Key__ __Concepts__<br> - System Compiler<br>__Keywords__<br> - create_bufpool<br> - alloc_buff<br> - compute<br> - send_while<br> - receive_all_in_order<br> - get_buf<br> - DATA_COPY<br> - SYS_PORT<br> - SYS_PORT_PFM<br> - ACCESS_PATTERN

[.]:.
[debug_profile_sc/]:debug_profile_sc/
[quick_start_sc/]:quick_start_sc/
