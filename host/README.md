Host Examples
==================================
OpenCL host code for optimized interfacing with Xilinx Devices.

 __Examples Table__ 

Example        | Description           | Key Concepts / Keywords 
---------------|-----------------------|-------------------------
[concurrent_kernel_execution/][]|This example will demonstrate how to use multiple and out of order command queues to simultaneously execute multiple kernels on an FPGA.|__Key__ __Concepts__<br> - Concurrent execution<br> - Out of Order Command Queues<br> - Multiple Command Queues<br>__Keywords__<br> - CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE<br> - setCallback
[copy_buffer/][]|This Copy Buffer example demonstrate how one buffer can be copied from another buffer.|__Key__ __Concepts__<br> - Copy Buffer<br>__Keywords__<br> - cl::CommandQueue<br> - enqueueCopyBuffer<br> - enqueueWriteBuffer<br> - enqueueReadBuffer<br> - enqueueMigrateMemObjects
[data_transfer/][]|This example illustrates several ways to use the OpenCL API to transfer data to and from the FPGA|__Key__ __Concepts__<br> - OpenCL API<br> - Data Transfer<br> - Write Buffers<br> - Read Buffers<br> - Map Buffers<br> - Async Memcpy<br>__Keywords__<br> - enqueueWriteBuffer<br> - enqueueReadBuffer<br> - enqueueMapBuffer<br> - enqueueUnmapMemObject<br> - enqueueMigrateMemObjects
[debug_profile/][]|This is simple example of vector addition and printing profile data (wall clock time taken between start and stop). It also dump a waveform file which can be reloaded to vivado to see the waveform. Run command 'vivado -source ./scripts/open_waveform.tcl -tclargs <device_name>-<kernel_name>.<target>.<device_name>.wdb' to launch waveform viewer. User can also update batch to gui in xrt.ini file to see the live waveform while running application.|__Key__ __Concepts__<br> - Use of Profile API<br> - Waveform Dumping and loading<br>
[device_only_buffer/][]|This example will demonstrate how to create buffers in global memory which are not mapped to host.|__Key__ __Concepts__<br> - Device only buffer<br>__Keywords__<br> - CL_MEM_HOST_NO_ACCESS
[device_query/][]|This Example prints the OpenCL properties of the platform and its devices using OpenCL CPP APIs. It also displays the limits and capabilities of the hardware.|__Key__ __Concepts__<br> - OpenCL API<br> - Querying device properties<br>
[errors/][]|This example discuss the different reasons for errors in OpenCL and how to handle them at runtime.|__Key__ __Concepts__<br> - OpenCL API<br> - Error handling<br>__Keywords__<br> - CL_SUCCESS<br> - CL_DEVICE_NOT_FOUND<br> - CL_DEVICE_NOT_AVAILABLE
[errors_cpp/][]|This example discuss the different reasons for errors in OpenCL C++ and how to handle them at runtime.|__Key__ __Concepts__<br> - OpenCL C++ API<br> - Error handling<br>__Keywords__<br> - CL_SUCCESS<br> - CL_DEVICE_NOT_FOUND<br> - CL_DEVICE_NOT_AVAILABLE<br> - CL_INVALID_VALUE<br> - CL_INVALID_KERNEL_NAME<br> - CL_INVALID_BUFFER_SIZE
[hbm_bandwidth/][]|This is a HBM bandwidth check design. Design contains 3 compute units of a kernel which has access to all HBM banks (0:31). Host application allocate buffer into all HBM banks and run these 3 compute units concurrently and measure the overall bandwidth between Kernel and HBM Memory.|
[hbm_simple/][]|This is a simple example of vector addition to describe how to use HLS kernels with HBM (High Bandwidth Memory) for achieving high throughput.|__Key__ __Concepts__<br> - High Bandwidth Memory<br> - Multiple HBM Banks<br>__Keywords__<br> - HBM<br> - XCL_MEM_TOPOLOGY<br> - cl_mem_ext_ptr_t
[host_global_bandwidth/][]|Host to global memory bandwidth test|
[mult_compute_units/][]|This is simple Example of Multiple Compute units to showcase how a single kernel can be instantiated into Multiple compute units. Host code will show how to use multiple compute units and run them concurrently.|__Key__ __Concepts__<br> - Multiple Compute Units<br>__Keywords__<br> - nk
[multiple_cus_asymmetrical/][]|This is simple example of vector addition to demonstrate how to connect each compute unit to different banks and how to use these compute units in host applications|__Key__ __Concepts__<br> - Multiple Compute Units<br>__Keywords__<br> - #pragma HLS PIPELINE
[overlap/][]|This examples demonstrates techniques that allow user to overlap Host(CPU) and FPGA computation in an application. It will cover asynchronous operations and event object.|__Key__ __Concepts__<br> - OpenCL API<br> - Synchronize Host and FPGA<br> - Asynchronous Processing<br> - Events<br> - Asynchronous memcpy<br>__Keywords__<br> - cl_event<br> - cl::CommandQueue<br> - CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE<br> - enqueueMigrateMemObjects
[p2p_bandwidth/][]|This is simple example to test Synchronous and Asyncronous data transfer between SSD and FPGA.|__Key__ __Concepts__<br> - P2P<br> - SSD<br> - XDMA<br>__Keywords__<br> - XCL_MEM_EXT_P2P_BUFFER<br> - pread<br> - pwrite
[p2p_simple/][]|This is simple example of vector increment to describe P2P between FPGA and NVMe SSD.|__Key__ __Concepts__<br> - P2P<br> - NVMe SSD<br>__Keywords__<br> - XCL_MEM_EXT_P2P_BUFFER<br> - pread<br> - pwrite<br> - O_DIRECT<br> - O_RDWR
[streaming_free_running_kernel/][]|This is simple example which demonstrate how to use and configure a free running kernel.|__Key__ __Concepts__<br> - Free Running Kernel<br>__Keywords__<br> - ap_ctrl_none<br> - stream_connect
[streaming_host_bandwidth/][]|This is a simple Vector Increment C Kernel design with 1 Stream input and 1 Stream output that demonstrates on how to process an input stream of data for computation in an application and the Host to Device streaming bandwidth test.|__Key__ __Concepts__<br> - Read/Write Stream<br> - Create/Release Stream<br>__Keywords__<br> - cl_stream<br> - CL_STREAM_EOT<br> - CL_STREAM_NONBLOCKING
[streaming_k2k/][]|This is a simple kernel to kernel streaming Vector Add and Vector Multiply C Kernel that demonstrates on how to process a stream of data for computation between two kernels.|__Key__ __Concepts__<br> - Read/Write Stream<br> - Create/Release Stream<br>__Keywords__<br> - cl_stream<br> - CL_STREAM_EOT<br> - CL_STREAM_NONBLOCKING
[streaming_k2k_mm/][]|This is a simple kernel to kernel streaming Vector Add and Vector Multiply C Kernel design with 2 memory mapped input to kernel 1, 1 Stream output from kernel 1 to input of kernel 2, 1 memory mapped input to kernel 2, and 1 memory mapped output that demonstrates on how to process a stream of data for computation between two kernels.|__Key__ __Concepts__<br> - Read/Write Stream<br> - Create/Release Stream<br>__Keywords__<br> - stream_connect
[streaming_mm_mixed/][]|This is a simple streaming Vector Addition C Kernel design with 1 Stream input, 1 memory mapped input to the kernel, and 1 stream output that demonstrates on how to process a stream of data along with memory mapped interface.|__Key__ __Concepts__<br> - Read/Write Stream<br> - Create/Release Stream<br>__Keywords__<br> - cl_stream<br> - CL_STREAM_EOT
[streaming_multi_cus/][]|This is a simple vadd Kernel design in which multiple compute units of kernel are directly communicating with host using stream interfaces.|__Key__ __Concepts__<br> - Read/Write Stream<br> - Create/Release Stream<br>__Keywords__<br> - cl_stream<br> - CL_STREAM_EOT<br> - nk
[streaming_simple/][]|This is a simple Vector Add C Kernel design with 2 Stream inputs and 1 Stream output that demonstrates on how streaming kernel can be implemented and how host can directly send data to kernel without global memory.|__Key__ __Concepts__<br> - Read/Write Stream<br> - Create/Release Stream<br>__Keywords__<br> - cl_stream<br> - CL_STREAM_EOT<br> - CL_STREAM_NONBLOCKING

[.]:.
[concurrent_kernel_execution/]:concurrent_kernel_execution/
[copy_buffer/]:copy_buffer/
[data_transfer/]:data_transfer/
[debug_profile/]:debug_profile/
[device_only_buffer/]:device_only_buffer/
[device_query/]:device_query/
[errors/]:errors/
[errors_cpp/]:errors_cpp/
[hbm_bandwidth/]:hbm_bandwidth/
[hbm_simple/]:hbm_simple/
[host_global_bandwidth/]:host_global_bandwidth/
[mult_compute_units/]:mult_compute_units/
[multiple_cus_asymmetrical/]:multiple_cus_asymmetrical/
[overlap/]:overlap/
[p2p_bandwidth/]:p2p_bandwidth/
[p2p_simple/]:p2p_simple/
[streaming_free_running_kernel/]:streaming_free_running_kernel/
[streaming_host_bandwidth/]:streaming_host_bandwidth/
[streaming_k2k/]:streaming_k2k/
[streaming_k2k_mm/]:streaming_k2k_mm/
[streaming_mm_mixed/]:streaming_mm_mixed/
[streaming_multi_cus/]:streaming_multi_cus/
[streaming_simple/]:streaming_simple/
