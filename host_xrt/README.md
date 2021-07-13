Native XRT Examples
==================================
XRT Native API's for optimized interfacing with Xilinx Devices.

 __Examples Table__ 

Example        | Description           | Key Concepts / Keywords 
---------------|-----------------------|-------------------------
[copy_buffer_xrt/][]|This Copy Buffer example demonstrate how one buffer can be copied from another buffer.|__Key__ __Concepts__<br> - Copy Buffer<br>__Keywords__<br> - copy
[data_transfer_xrt/][]|This example illustrates transferring of data to and from the FPGA using xrt native api's|__Key__ __Concepts__<br> - [XRT Native API](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devhostapp.html#jln1620691667890)<br> - [Data Transfer](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/opencl_programming.html#xio1524524087132)<br> - [Write Buffers](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/opencl_programming.html#xio1524524087132)<br> - [Read Buffers](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/opencl_programming.html#xio1524524087132)<br>__Keywords__<br> - [xrt::bo::write](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devhostapp.html#yaz1622072496185)<br> - [xrt::bo::read](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devhostapp.html#yaz1622072496185)
[device_only_buffer_xrt/][]|This example will demonstrate how to create buffers in global memory which are not mapped to host.|__Key__ __Concepts__<br> - Device only buffer<br>__Keywords__<br> - xrt::bo::flags::device_only
[device_query_xrt/][]|This Example prints device properties using XRT Native APIs. It also displays the limits and capabilities of the hardware.|__Key__ __Concepts__<br> - Querying device properties<br>__Keywords__<br> - [xrt::device](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devhostapp.html#zja1524097906844)
[hbm_simple_xrt/][]|This is a simple example of vector addition to describe how to use HLS kernels with HBM (High Bandwidth Memory) for achieving high throughput using xrt native api's.|__Key__ __Concepts__<br> - [High Bandwidth Memory](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/buildingdevicebinary.html#lgl1614021146997)<br> - Multiple HBM Banks<br>__Keywords__<br> - [HBM](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/buildingdevicebinary.html#lgl1614021146997)
[hello_world_xrt/][]|This is simple example of vector addition to describe the usage of XRT Native API's.|__Key__ __Concepts__<br> - [XRT Native API](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devhostapp.html#jln1620691667890)<br>__Keywords__<br> - [xrt::device](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devhostapp.html#zja1524097906844)<br> - load_xclbin<br> - [xrt::bo](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devhostapp.html#yaz1622072496185)<br> - [xrt::kernel](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devhostapp.html#lqq1622157666187)<br> - [map](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devhostapp.html#yaz1622072496185)<br> - [sync](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devhostapp.html#yaz1622072496185)<br> - XCL_BO_SYNC_BO_TO_DEVICE<br> - XCL_BO_SYNC_BO_FROM_DEVICE
[iops_fast_adapter_xrt/][]|This is simple test design to measure Input/Output Operations per second using Fast Adapter. In this design, a simple kernel is enqueued many times and overall IOPS is measured using XRT native api's.|__Key__ __Concepts__<br> - Input/Output Operations per second<br> - Fast Adapter<br>__Keywords__<br> - nextDescriptorAddr
[iops_test_xrt/][]|This is simple test design to measure Input/Output Operations per second. In this design, a simple kernel is enqueued many times and measuring overall IOPS using XRT native api's.|__Key__ __Concepts__<br> - Input/Output Operations per second<br>
[mult_compute_units_xrt/][]|This is simple Example of Multiple Compute units to showcase how a single kernel can be instantiated into Multiple compute units. Host code will show how to use multiple compute units and run them concurrently using XRT Native api's.|__Key__ __Concepts__<br> - [Multiple compute units](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/opencl_programming.html#dqz1555367565037)<br>__Keywords__<br> - [nk](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitiscommandcompiler.html#clt1568640709907__section_wtp_zvm_1jb)
[multiple_cus_asymmetrical_xrt/][]|This is simple example of vector addition to demonstrate how to connect each compute unit to different banks and how to use these compute units in host applications using xrt native api's.|__Key__ __Concepts__<br> - [Multiple compute units](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/opencl_programming.html#dqz1555367565037)<br>
[p2p_simple_xrt/][]|This is simple example of vector increment to describe P2P between FPGA and NVMe SSD using xrt native api's.|__Key__ __Concepts__<br> - [P2P](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/Chunk938767849.html#qex1558551641915)<br> - NVMe SSD<br> - SmartSSD<br>__Keywords__<br> - xrt::bo::flags::p2p<br> - pread<br> - pwrite<br> - O_DIRECT<br> - O_RDWR
[register_access_xrt/][]|This is simple example which demonstrate register read & write using native xrt api's.|__Keywords__<br> - read_register<br> - write_register<br> - cu_access_mode<br> - exclusive
[slave_bridge_bandwidth_xrt/][]|This is slave bridge bandwidth example to describe host memory and kernel bandwidth test using native xrt api's.|__Key__ __Concepts__<br> - slave bridge<br> - bandwidth<br> - address translation unit<br>__Keywords__<br> - host_only<br> - HOST[0]
[slave_bridge_copy_buffer_xrt/][]|This is simple slave bridge example to describe how host-only memory can be copied to device-only memory and vice-versa using XRT Native API's.|__Key__ __Concepts__<br> - slave bridge<br>__Keywords__<br> - host_only<br> - device_only<br> - HOST[0]
[slave_bridge_copy_kernel_xrt/][]|This is a Slave Bridge Example to describe how data can be copied using xrt native api's between host-only buffer and device-only buffer using User Copy Kernel.|__Key__ __Concepts__<br> - slave bridge<br>__Keywords__<br> - host_only<br> - device_only
[slave_bridge_simple_xrt/][]|This is simple slave bridge example to describe how a user kernel can access the host memory using xrt native api's.|__Key__ __Concepts__<br> - slave bridge<br> - address translation unit<br>__Keywords__<br> - host_only<br> - HOST[0]
[streaming_free_running_k2k_xrt/][]|This is simple example which demonstrate how to use and configure a free running kernel.|__Key__ __Concepts__<br> - [Free Running Kernel](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/streamingconnections.html#uug1556136182736)<br>__Keywords__<br> - [ap_ctrl_none](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/managing_interface_synthesis.html#qls1539734256651__ae476284)<br> - [stream_connect](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/buildingdevicebinary.html#yha1565541199876)
[streaming_k2k_mm_xrt/][]|This is a simple kernel to kernel streaming Vector Add and Vector Multiply C Kernel design with 2 memory mapped input to kernel 1, 1 Stream output from kernel 1 to input of kernel 2, 1 memory mapped input to kernel 2, and 1 memory mapped output that demonstrates on how to process a stream of data for computation between two kernels using XRT Native APIs. This design also illustrates how to set FIFO depth for AXIS connections i.e. for the stream connecting the two kernels|__Key__ __Concepts__<br> - [Read/Write Stream](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/streamingconnections.html#ynb1556233012018)<br> - [Create/Release Stream](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/streamingconnections.html#ynb1556233012018)<br> - [AXIS FIFO depth](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitis_hls_optimization_techniques.html#tvy1539734226664)<br>__Keywords__<br> - [stream_connect](https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/buildingdevicebinary.html#yha1565541199876)

[.]:.
[copy_buffer_xrt/]:copy_buffer_xrt/
[data_transfer_xrt/]:data_transfer_xrt/
[device_only_buffer_xrt/]:device_only_buffer_xrt/
[device_query_xrt/]:device_query_xrt/
[hbm_simple_xrt/]:hbm_simple_xrt/
[hello_world_xrt/]:hello_world_xrt/
[iops_fast_adapter_xrt/]:iops_fast_adapter_xrt/
[iops_test_xrt/]:iops_test_xrt/
[mult_compute_units_xrt/]:mult_compute_units_xrt/
[multiple_cus_asymmetrical_xrt/]:multiple_cus_asymmetrical_xrt/
[p2p_simple_xrt/]:p2p_simple_xrt/
[register_access_xrt/]:register_access_xrt/
[slave_bridge_bandwidth_xrt/]:slave_bridge_bandwidth_xrt/
[slave_bridge_copy_buffer_xrt/]:slave_bridge_copy_buffer_xrt/
[slave_bridge_copy_kernel_xrt/]:slave_bridge_copy_kernel_xrt/
[slave_bridge_simple_xrt/]:slave_bridge_simple_xrt/
[streaming_free_running_k2k_xrt/]:streaming_free_running_k2k_xrt/
[streaming_k2k_mm_xrt/]:streaming_k2k_mm_xrt/
