Host Examples
==================================
OpenCL host code for optimized interfacing with Xilinx Devices.

 __Examples Table__ 

Example        | Description           | Key Concepts / Keywords 
---------------|-----------------------|-------------------------
[concurrent_kernel_execution/][]|This example will demonstrate how to use multiple and out of order command queues to simultaneously execute multiple kernels on an FPGA.|__Key__ __Concepts__<br> - Concurrent execution<br> - Out of Order Command Queues<br> - Multiple Command Queues<br>__Keywords__<br> - CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE<br> - clSetEventCallback()
[copy_buffer/][]|This Copy Buffer example demonstrate how one buffer can be copied from another buffer.|__Key__ __Concepts__<br> - Copy Buffer<br>__Keywords__<br> - cl::CommandQueue::enqueueCopyBuffer()
[data_transfer/][]|This example illustrates several ways to use the OpenCL API to transfer data to and from the FPGA|__Key__ __Concepts__<br> - OpenCL API<br> - Data Transfer<br> - Write Buffers<br> - Read Buffers<br> - Map Buffers<br> - Async Memcpy<br>__Keywords__<br> - enqueueWriteBuffer()<br> - enqueueReadBuffer()<br> - enqueueMapBuffer()<br> - enqueueUnmapMemObject()<br> - enqueueMigrateMemObjects()
[device_query/][]|This example prints the OpenCL properties of the platform and its devices. It also displays the limits and capabilities of the hardware.|__Key__ __Concepts__<br> - OpenCL API<br> - Querying device properties<br>__Keywords__<br> - clGetPlatformIDs()<br> - clGetPlatformInfo()<br> - clGetDeviceIDs()<br> - clGetDeviceInfo()
[device_query_cpp/][]|This Example prints the OpenCL properties of the platform and its devices using OpenCL CPP APIs. It also displays the limits and capabilities of the hardware.|__Key__ __Concepts__<br> - OpenCL API<br> - Querying device properties<br>
[errors/][]|This example discuss the different reasons for errors in OpenCL and how to handle them at runtime.|__Key__ __Concepts__<br> - OpenCL API<br> - Error handling<br>__Keywords__<br> - CL_SUCCESS<br> - CL_DEVICE_NOT_FOUND<br> - CL_DEVICE_NOT_AVAILABLE
[errors_cpp/][]|This example discuss the different reasons for errors in OpenCL C++ and how to handle them at runtime.|__Key__ __Concepts__<br> - OpenCL C++ API<br> - Error handling<br>__Keywords__<br> - CL_SUCCESS<br> - CL_DEVICE_NOT_FOUND<br> - CL_DEVICE_NOT_AVAILABLE<br> - CL_INVALID_VALUE<br> - CL_INVALID_KERNEL_NAME<br> - CL_INVALID_BUFFER_SIZE
[hbm_bandwidth/][]|This is a HBM bandwidth check design. Design contains 8 compute units of a kernel which has access to all HBM banks (0:31). Host application allocate buffer into all HBM banks and run these 8 compute units concurrently and measure the overall bandwidth between Kernel and HBM Memory.|
[hbm_simple/][]|This is a simple example of vector addition to describe how to use HLS kernels with HBM (High Bandwidth Memory) for achieving high throughput.|__Key__ __Concepts__<br> - High Bandwidth Memory<br> - Multiple HBM Banks<br>__Keywords__<br> - HBM<br> - XCL_MEM_TOPOLOGY<br> - cl_mem_ext_ptr_t
[host_global_bandwidth/][]|Host to global memory bandwidth test|
[host_global_bandwidth_5.0_shell/][]|Host to global memory bandwidth test for 5.0 shell|
[kernel_swap/][]|This example shows how host can swap the kernels and share same buffer between two kernels which are exist in separate binary containers. Dynamic platforms does not persist the buffer data so host has to migrate data from device to host memory before swapping the next kernel. After kernel swap, host has to migrate the buffer back to device.|__Key__ __Concepts__<br> - Handling Buffer sharing across multiple binaries<br> - Multiple Kernel Binaries<br>__Keywords__<br> - clEnqueueMigrateMemObjects()<br> - CL_MIGRATE_MEM_OBJECT_HOST
[multiple_cus_asymmetrical/][]|This is simple example of vector addition to demonstrate how to connect each compute unit to different banks and how to use these compute units in host applications|__Key__ __Concepts__<br> - Multiple Compute Units<br>__Keywords__<br> - #pragma HLS PIPELINE
[multiple_devices/][]|This example show how to take advantage of multiple FPGAs on a system. It will show how to initialized an OpenCL context, allocate memory on the two devices and execute a kernel on each FPGA.|__Key__ __Concepts__<br> - OpenCL API<br> - Multi-FPGA Execution<br> - Event Handling<br>__Keywords__<br> - cl_device_id<br> - clGetDeviceIDs()
[multiple_process/][]|This example will demonstrate how to run multiple processes to utilize multiple kernels simultaneously on an FPGA device. Multiple processes can share access to the same device provided each process uses the same xclbin. Processes share access to all device resources but there is no support for exclusive access to resources by any process.|__Key__ __Concepts__<br> - Concurrent execution<br> - Multiple HLS kernels<br> - Multiple Process Support<br>__Keywords__<br> - PID<br> - fork<br> - XCL_MULTIPROCESS_MODE<br> - multiprocess
[overlap/][]|This examples demonstrates techniques that allow user to overlap Host(CPU) and FPGA computation in an application. It will cover asynchronous operations and event object.|__Key__ __Concepts__<br> - OpenCL API<br> - Synchronize Host and FPGA<br> - Asynchronous Processing<br> - Events<br> - Asynchronous memcpy<br>__Keywords__<br> - cl_event<br> - clCreateCommandQueue<br> - CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE<br> - clEnqueueMigrateMemObjects
[streaming_host_bandwidth/][]|This is a simple Vector Increment C Kernel design with 1 Stream input and 1 Stream output that demonstrates on how to process an input stream of data for computation in an application and the Host to Device streaming bandwidth test.|__Key__ __Concepts__<br> - Read/Write Stream<br> - Create/Release Stream<br>__Keywords__<br> - cl_stream<br> - CL_STREAM_EOT<br> - CL_STREAM_NONBLOCKING
[streaming_k2k/][]|This is a simple kernel to kernel streaming Vector Add and Vector Multiply C Kernel that demonstrates on how to process a stream of data for computation between two kernels.|__Key__ __Concepts__<br> - Read/Write Stream<br> - Create/Release Stream<br>__Keywords__<br> - cl_stream<br> - CL_STREAM_EOT<br> - CL_STREAM_NONBLOCKING
[streaming_k2k_mm/][]|This is a simple kernel to kernel streaming Vector Add and Vector Multiply C Kernel design with 2 memory mapped input to kernel 1, 1 Stream output from kernel 1 to input of kernel 2, 1 memory mapped input to kernel 2, and 1 memory mapped output that demonstrates on how to process a stream of data for computation between two kernels.|__Key__ __Concepts__<br> - Read/Write Stream<br> - Create/Release Stream<br>__Keywords__<br> - cl_stream<br> - CL_STREAM_EOT<br> - CL_STREAM_BLOCKING
[streaming_mm_mixed/][]|This is a simple streaming Vector Addition C Kernel design with 1 Stream input, 1 memory mapped input to the kernel, and 1 stream output that demonstrates on how to process a stream of data for computation along with OpenCL buffers.|__Key__ __Concepts__<br> - Read/Write Stream<br> - Create/Release Stream<br>__Keywords__<br> - cl_stream<br> - CL_STREAM_EOT<br> - CL_STREAM_BLOCKING
[streaming_multi_cus/][]|This is a simple Vector Add C Kernel design with 2 Stream inputs and 1 Stream output that demonstrates on how to process an input stream of data for computation in an application using multiple compute units.|__Key__ __Concepts__<br> - Read/Write Stream<br> - Create/Release Stream<br>__Keywords__<br> - cl_stream<br> - CL_STREAM_EOT<br> - Multiple Compute Units
[streaming_simple/][]|This is a simple Vector Add C Kernel design with 2 Stream inputs and 1 Stream output that demonstrates on how to process an input stream of data for computation in an application.|__Key__ __Concepts__<br> - Read/Write Stream<br> - Create/Release Stream<br>__Keywords__<br> - cl_stream<br> - CL_STREAM_EOT<br> - CL_STREAM_NONBLOCKING
[sub_devices/][]|This example demonstrates how to create OpenCL subdevices which uses a single kernel multiple times in order to show how to handle each instance independently including independent buffers, command queues and sequencing.|__Key__ __Concepts__<br> - Sub Devices<br>__Keywords__<br> - cl_device_partition_property<br> - createSubDevices<br> - CL_DEVICE_PARTITION_EQUALLY

[.]:.
[concurrent_kernel_execution/]:concurrent_kernel_execution/
[copy_buffer/]:copy_buffer/
[data_transfer/]:data_transfer/
[device_query/]:device_query/
[device_query_cpp/]:device_query_cpp/
[errors/]:errors/
[errors_cpp/]:errors_cpp/
[hbm_bandwidth/]:hbm_bandwidth/
[hbm_simple/]:hbm_simple/
[host_global_bandwidth/]:host_global_bandwidth/
[host_global_bandwidth_5.0_shell/]:host_global_bandwidth_5.0_shell/
[kernel_swap/]:kernel_swap/
[multiple_cus_asymmetrical/]:multiple_cus_asymmetrical/
[multiple_devices/]:multiple_devices/
[multiple_process/]:multiple_process/
[overlap/]:overlap/
[streaming_host_bandwidth/]:streaming_host_bandwidth/
[streaming_k2k/]:streaming_k2k/
[streaming_k2k_mm/]:streaming_k2k_mm/
[streaming_mm_mixed/]:streaming_mm_mixed/
[streaming_multi_cus/]:streaming_multi_cus/
[streaming_simple/]:streaming_simple/
[sub_devices/]:sub_devices/
