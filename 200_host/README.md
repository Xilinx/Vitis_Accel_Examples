Host Examples
==================================
OpenCL host code for optimized interfacing with Xilinx Devices

 __Examples Table__ 

Example        | Description           | Key Concepts / Keywords 
---------------|-----------------------|-------------------------
[201_device_query/][]|This example prints the OpenCL properties of the platform and its devices. It also displays the limits and capabilities of the hardware.|__Key__ __Concepts__<br> - OpenCL API<br> - Querying device properties<br>__Keywords__<br> - clGetPlatformIDs()<br> - clGetPlatformInfo()<br> - clGetDeviceIDs()<br> - clGetDeviceInfo()
[202_device_query_cpp/][]|This Example prints the OpenCL properties of the platform and its devices using OpenCL CPP APIs. It also displays the limits and capabilities of the hardware.|__Key__ __Concepts__<br> - OpenCL API<br> - Querying device properties<br>
[203_errors/][]|This example discuss the different reasons for errors in OpenCL and how to handle them at runtime.|__Key__ __Concepts__<br> - OpenCL API<br> - Error handling<br>__Keywords__<br> - CL_SUCCESS<br> - CL_DEVICE_NOT_FOUND<br> - CL_DEVICE_NOT_AVAILABLE
[204_errors_cpp/][]|This example discuss the different reasons for errors in OpenCL C++ and how to handle them at runtime.|__Key__ __Concepts__<br> - OpenCL C++ API<br> - Error handling<br>__Keywords__<br> - CL_SUCCESS<br> - CL_DEVICE_NOT_FOUND<br> - CL_DEVICE_NOT_AVAILABLE<br> - CL_INVALID_VALUE<br> - CL_INVALID_KERNEL_NAME<br> - CL_INVALID_BUFFER_SIZE
[205_data_transfer/][]|This example illustrates several ways to use the OpenCL API to transfer data to and from the FPGA|__Key__ __Concepts__<br> - OpenCL API<br> - Data Transfer<br> - Write Buffers<br> - Read Buffers<br> - Map Buffers<br> - Async Memcpy<br>__Keywords__<br> - enqueueWriteBuffer()<br> - enqueueReadBuffer()<br> - enqueueMapBuffer()<br> - enqueueUnmapMemObject()<br> - enqueueMigrateMemObjects()
[206_host_global_bandwidth/][]|Host to global memory bandwidth test|
[207_concurrent_kernel_execution/][]|This example will demonstrate how to use multiple and out of order command queues to simultaneously execute multiple kernels on an FPGA.|__Key__ __Concepts__<br> - Concurrent execution<br> - Out of Order Command Queues<br> - Multiple Command Queues<br>__Keywords__<br> - CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE<br> - clSetEventCallback()
[208_kernel_swap/][]|This example shows how host can swap the kernels and share same buffer between two kernels which are exist in separate binary containers. Dynamic platforms does not persist the buffer data so host has to migrate data from device to host memory before swapping the next kernel. After kernel swap, host has to migrate the buffer back to device.|__Key__ __Concepts__<br> - Handling Buffer sharing across multiple binaries<br> - Multiple Kernel Binaries<br>__Keywords__<br> - clEnqueueMigrateMemObjects()<br> - CL_MIGRATE_MEM_OBJECT_HOST
[209_overlap/][]|This examples demonstrates techniques that allow user to overlap Host(CPU) and FPGA computation in an application. It will cover asynchronous operations and event object.|__Key__ __Concepts__<br> - OpenCL API<br> - Synchronize Host and FPGA<br> - Asynchronous Processing<br> - Events<br> - Asynchronous memcpy<br>__Keywords__<br> - cl_event<br> - clCreateCommandQueue<br> - CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE<br> - clEnqueueMigrateMemObjects
[210_copy_buffer/][]|This Copy Buffer example demonstrate how one buffer can be copied from another buffer.|__Key__ __Concepts__<br> - Copy Buffer<br>__Keywords__<br> - cl::CommandQueue::enqueueCopyBuffer()
[211_multiple_process/][]|This example will demonstrate how to run multiple processes to utilize multiple kernels simultaneously on an FPGA device. Multiple processes can share access to the same device provided each process uses the same xclbin. Processes share access to all device resources but there is no support for exclusive access to resources by any process.|__Key__ __Concepts__<br> - Concurrent execution<br> - Multiple HLS kernels<br> - Multiple Process Support<br>__Keywords__<br> - PID<br> - fork<br> - XCL_MULTIPROCESS_MODE<br> - multiprocess
[212_sub_devices/][]|This example demonstrates how to create OpenCL subdevices which uses a single kernel multiple times in order to show how to handle each instance independently including independent buffers, command queues and sequencing.|__Key__ __Concepts__<br> - Sub Devices<br>__Keywords__<br> - cl_device_partition_property<br> - createSubDevices<br> - CL_DEVICE_PARTITION_EQUALLY
[213_stream_access/][]|This is a simple example that demonstrates on how to process an input stream of data for computation in an application. It shows how to perform asynchronous operations and event handling.|__Key__ __Concepts__<br> - OpenCL API<br> - Synchronize Host and FPGA<br> - Asynchronous Processing<br> - Events<br> - Asynchronous Data Transfer<br>__Keywords__<br> - cl::event<br> - CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE

[.]:.
[201_device_query/]:201_device_query/
[202_device_query_cpp/]:202_device_query_cpp/
[203_errors/]:203_errors/
[204_errors_cpp/]:204_errors_cpp/
[205_data_transfer/]:205_data_transfer/
[206_host_global_bandwidth/]:206_host_global_bandwidth/
[207_concurrent_kernel_execution/]:207_concurrent_kernel_execution/
[208_kernel_swap/]:208_kernel_swap/
[209_overlap/]:209_overlap/
[210_copy_buffer/]:210_copy_buffer/
[211_multiple_process/]:211_multiple_process/
[212_sub_devices/]:212_sub_devices/
[213_stream_access/]:213_stream_access/
