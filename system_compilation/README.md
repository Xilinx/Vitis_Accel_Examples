System Compiler Examples
==================================
This section contains System Compiler based Examples.

 __Examples Table__ 

Example        | Description           | Key Concepts / Keywords 
---------------|-----------------------|-------------------------
[debug_profile_sc/][]|This simple example implements a HW pipeline of three tasks, matrix multiplication along with two different increments to the matrix values.|__Key__ __Concepts__<br> - Debug<br> - Profiling<br>__Keywords__<br> - PROFILE_KERNEL<br> - PROFILE_PORT<br> - PROFILE_OFFLOAD<br> - vpp::stream<>
[file_filter_sc/][]|This is simple example showcasing the P2P and H2C file transfers in system compiler environment.|__Key__ __Concepts__<br> - [P2P](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/p2p)<br> - SSD<br> - Syncronization<br>__Keywords__<br> - vpp::p2p<br> - vpp::h2c<br> - file_buf<br> - sync_output<br> - custom_sync_outputs<br> - sync_output_to_file
[gmio_transfers_sc/][]|This is gmio transfers example which explains different ways of data movements between host and device for compute unit processing.|__Key__ __Concepts__<br> - System Compiler<br>__Keywords__<br> - DATA_COPY<br> - ZERO_COPY<br> - ACCESS_PATTERN<br> - SEQUENTIAL<br> - RANDOM
[mult_acc_compose_sc/][]|In this example we have multiple accelerators in one xclbin, and compose them into a pipeline, with or without CPU processing in-between the PEs.|__Key__ __Concepts__<br> - Multiple Accelerators<br>__Keywords__<br> - sync_outputs<br> - custom_sync_outputs
[quick_start_sc/][]|This is quick start example based on a video filtering usecase to describe system compiler usage in Vitis Environment.|__Key__ __Concepts__<br> - System Compiler<br>__Keywords__<br> - create_bufpool<br> - alloc_buff<br> - compute<br> - send_while<br> - receive_all_in_order<br> - get_buf<br> - DATA_COPY<br> - SYS_PORT<br> - SYS_PORT_PFM<br> - ACCESS_PATTERN

[.]:.
[debug_profile_sc/]:debug_profile_sc/
[file_filter_sc/]:file_filter_sc/
[gmio_transfers_sc/]:gmio_transfers_sc/
[mult_acc_compose_sc/]:mult_acc_compose_sc/
[quick_start_sc/]:quick_start_sc/
