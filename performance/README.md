Performance Examples
==================================
List of examples that cover performance related aspect.

 __Examples Table__ 

Example        | Description           | Key Concepts / Keywords 
---------------|-----------------------|-------------------------
[axi_burst_performance/][]|This is an AXI Burst Performance check design. It measures the time it takes to write a buffer into DDR or read a buffer from DDR. The example contains 2 sets of 6 kernels each: each set having a different data width and each kernel having a different burst_length and num_outstanding parameters to compare the impact of these parameters on effective throughput.|
[hbm_bandwidth/][]|This is a HBM bandwidth check design. Design contains 3 compute units of a kernel which has access to all HBM pseudo-channels (0:31). Host application allocate buffer into all HBM banks and run these 3 compute units concurrently and measure the overall bandwidth between Kernel and HBM Memory.|
[hbm_bandwidth_pseudo_random/][]|This is a HBM bandwidth example using a pseudo random 1024 bit data access pattern to mimic Ethereum Ethash workloads. The design contains 3 compute units of a kernel, reading 1024 bits from a pseudo random address in each of 2 pseudo channels and writing the results of a simple mathematical operation to a pseudo random address in 2 other pseudo channels. To maximize bandwidth the pseudo channels are used in  P2P like configuration - See https://developer.xilinx.com/en/articles/maximizing-memory-bandwidth-with-vitis-and-xilinx-ultrascale-hbm-devices.html for more information on HBM memory access configurations. The host application allocates buffers in 12  HBM banks and runs the compute units concurrently to measure the overall bandwidth between kernel and HBM Memory.|__Key__ __Concepts__<br> - [High Bandwidth Memory](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/HBM-Configuration-and-Use)<br> - [Multiple HBM Pseudo-channels](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/HBM-Configuration-and-Use)<br> - Random Memory Access<br> - Linear Feedback Shift Register<br>__Keywords__<br> - [HBM](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/HBM-Configuration-and-Use)<br> - [XCL_MEM_TOPOLOGY](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Assigning-DDR-Bank-in-Host-Code)<br> - [cl_mem_ext_ptr_t](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Assigning-DDR-Bank-in-Host-Code)
[host_global_bandwidth/][]|Host to global memory bandwidth test|
[host_memory_bandwidth/][]|This is host memory bandwidth example.|__Key__ __Concepts__<br> - host memory<br> - bandwidth<br> - address translation unit<br>__Keywords__<br> - XCL_MEM_EXT_HOST_ONLY<br> - HOST[0]
[host_memory_bandwidth_xrt/][]|This is host memory bandwidth example to describe host memory and kernel bandwidth test using native xrt api's.|__Key__ __Concepts__<br> - host memory<br> - bandwidth<br> - address translation unit<br>__Keywords__<br> - host_only<br> - HOST[0]
[iops_test_xrt/][]|This is simple test design to measure Input/Output Operations per second. In this design, a simple kernel is enqueued many times and measuring overall IOPS using XRT native api's.|__Key__ __Concepts__<br> - Input/Output Operations per second<br>
[kernel_global_bandwidth/][]|Bandwidth test of global to local memory.|
[p2p_fpga2fpga_bandwidth/][]|This is simple example to explain performance bandwidth for P2P transfer between two FPGA devices.|__Key__ __Concepts__<br> - [P2P](https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/p2p)<br> - Multi-FPGA Execution<br> - XDMA<br>__Keywords__<br> - XCL_MEM_EXT_P2P_BUFFER

[.]:.
[axi_burst_performance/]:axi_burst_performance/
[hbm_bandwidth/]:hbm_bandwidth/
[hbm_bandwidth_pseudo_random/]:hbm_bandwidth_pseudo_random/
[host_global_bandwidth/]:host_global_bandwidth/
[host_memory_bandwidth/]:host_memory_bandwidth/
[host_memory_bandwidth_xrt/]:host_memory_bandwidth_xrt/
[iops_test_xrt/]:iops_test_xrt/
[kernel_global_bandwidth/]:kernel_global_bandwidth/
[p2p_fpga2fpga_bandwidth/]:p2p_fpga2fpga_bandwidth/
