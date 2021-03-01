Native XRT Examples
==================================
XRT Native API's for optimized interfacing with Xilinx Devices.

 __Examples Table__ 

Example        | Description           | Key Concepts / Keywords 
---------------|-----------------------|-------------------------
[copy_buffer_xrt/][]|This Copy Buffer example demonstrate how one buffer can be copied from another buffer.|__Key__ __Concepts__<br> - Copy Buffer<br>__Keywords__<br> - copy
[data_transfer_xrt/][]|This example illustrates transferring of data to and from the FPGA using xrt native api's|__Key__ __Concepts__<br> - XRT Native API<br> - Data Transfer<br> - Write Buffers<br> - Read Buffers<br>__Keywords__<br> - xrt::bo::write<br> - xrt::bo::read
[device_only_buffer_xrt/][]|This example will demonstrate how to create buffers in global memory which are not mapped to host.|__Key__ __Concepts__<br> - Device only buffer<br>__Keywords__<br> - xrt::bo::flags::device_only
[hbm_simple_xrt/][]|This is a simple example of vector addition to describe how to use HLS kernels with HBM (High Bandwidth Memory) for achieving high throughput using xrt native api's.|__Key__ __Concepts__<br> - High Bandwidth Memory<br> - Multiple HBM Banks<br>__Keywords__<br> - HBM
[hello_world_xrt/][]|This is simple example of vector addition to describe the usage of XRT Native API's.|__Key__ __Concepts__<br> - XRT Native APIs<br>__Keywords__<br> - xrt::device<br> - load_xclbin<br> - xrt::bo<br> - xrt::kernel<br> - map<br> - sync<br> - XCL_BO_SYNC_BO_TO_DEVICE<br> - XCL_BO_SYNC_BO_FROM_DEVICE
[iops_test_xrt/][]|This is simple test design to measure Input/Output Operations per second. In this design, a simple kernel is enqueued many times and measuring overall IOPS using XRT native api's.|__Key__ __Concepts__<br> - Input/Output Operations per second<br>
[mult_compute_units_xrt/][]|This is simple Example of Multiple Compute units to showcase how a single kernel can be instantiated into Multiple compute units. Host code will show how to use multiple compute units and run them concurrently using XRT Native api's.|__Key__ __Concepts__<br> - Multiple Compute Units<br>__Keywords__<br> - nk
[multiple_cus_asymmetrical_xrt/][]|This is simple example of vector addition to demonstrate how to connect each compute unit to different banks and how to use these compute units in host applications using xrt native api's.|__Key__ __Concepts__<br> - Multiple Compute Units<br>
[p2p_simple_xrt/][]|This is simple example of vector increment to describe P2P between FPGA and NVMe SSD using xrt native api's.|__Key__ __Concepts__<br> - P2P<br> - NVMe SSD<br> - SmartSSD<br>__Keywords__<br> - xrt::bo::flags::p2p<br> - pread<br> - pwrite<br> - O_DIRECT<br> - O_RDWR
[register_access_xrt/][]|This is simple example which demonstrate register read & write using native xrt api's.|__Keywords__<br> - read_register<br> - write_register<br> - cu_access_mode<br> - exclusive
[slave_bridge_copy_buffer_xrt/][]|This is simple slave bridge example to describe how host-only memory can be copied to device-only memory and vice-versa using XRT Native API's.|__Key__ __Concepts__<br> - slave bridge<br>__Keywords__<br> - host_only<br> - device_only<br> - HOST[0]
[slave_bridge_simple_xrt/][]|This is simple slave bridge example to describe how a user kernel can access the host memory using xrt native api's.|__Key__ __Concepts__<br> - slave bridge<br> - address translation unit<br>__Keywords__<br> - host_only<br> - HOST[0]
[streaming_k2k_mm_xrt/][]|This is a simple kernel to kernel streaming Vector Add and Vector Multiply C Kernel design with 2 memory mapped input to kernel 1, 1 Stream output from kernel 1 to input of kernel 2, 1 memory mapped input to kernel 2, and 1 memory mapped output that demonstrates on how to process a stream of data for computation between two kernels using XRT Native APIs. This design also illustrates how to set FIFO depth for AXIS connections i.e. for the stream connecting the two kernels|__Key__ __Concepts__<br> - Read/Write Stream<br> - Create/Release Stream<br> - AXIS FIFO depth<br>__Keywords__<br> - stream_connect

[.]:.
[copy_buffer_xrt/]:copy_buffer_xrt/
[data_transfer_xrt/]:data_transfer_xrt/
[device_only_buffer_xrt/]:device_only_buffer_xrt/
[hbm_simple_xrt/]:hbm_simple_xrt/
[hello_world_xrt/]:hello_world_xrt/
[iops_test_xrt/]:iops_test_xrt/
[mult_compute_units_xrt/]:mult_compute_units_xrt/
[multiple_cus_asymmetrical_xrt/]:multiple_cus_asymmetrical_xrt/
[p2p_simple_xrt/]:p2p_simple_xrt/
[register_access_xrt/]:register_access_xrt/
[slave_bridge_copy_buffer_xrt/]:slave_bridge_copy_buffer_xrt/
[slave_bridge_simple_xrt/]:slave_bridge_simple_xrt/
[streaming_k2k_mm_xrt/]:streaming_k2k_mm_xrt/
