System Optimization Examples
==================================
System Optimization examples focussed towards hardware optimization using HLS C/C++ kernels for Xilinx devices.

 __Examples Table__ 

Example        | Description           | Key Concepts / Keywords 
---------------|-----------------------|-------------------------
[advanced_config/][]|This is simple example to describe how user can provide various advanced options in Vitis flow.|__Key__ __Concepts__<br> - Using custom Tcl<br> - Setting Kernel Clock Frequency<br> - Clock Uncertainity<br>__Keywords__<br> - config<br> - set_clock_uncertainity<br> - kernel_frequency<br> - hls_pre_tcl
[kernel_swap/][]|This example shows how host can swap the kernels and share same buffer between two kernels which are exist in separate binary containers. Dynamic platforms does not persist the buffer data so host has to migrate data from device to host memory before swapping the next kernel. After kernel swap, host has to migrate the buffer back to device.|__Key__ __Concepts__<br> - Handling Buffer sharing across multiple binaries<br> - Multiple Kernel Binaries<br>__Keywords__<br> - enqueueMigrateMemObjects<br> - CL_MIGRATE_MEM_OBJECT_HOST
[multiple_devices/][]|This example show how to take advantage of multiple FPGAs on a system. It will show how to initialized an OpenCL context, allocate memory on the two devices and execute a kernel on each FPGA.|__Key__ __Concepts__<br> - OpenCL API<br> - Multi-FPGA Execution<br> - Event Handling<br>__Keywords__<br> - xcl::get_xil_devices
[multiple_process/][]|This example will demonstrate how to run multiple processes to utilize multiple kernels simultaneously on an FPGA device. Multiple processes can share access to the same device provided each process uses the same xclbin. Processes share access to all device resources but there is no support for exclusive access to resources by any process.|__Key__ __Concepts__<br> - Concurrent execution<br> - Multiple HLS kernels<br> - Multiple Process Support<br>__Keywords__<br> - PID<br> - fork<br> - XCL_MULTIPROCESS_MODE<br> - multiprocess
[slr_assign/][]|This is simple example to describe SLR assignment information for a platform design. This example highlights how to provide extra input to assign the logic of the kernel into a nominated SLR. In this example we are assigning first kernel(Vector Multiplication) to SLR0 and assigning the second kernel(Vector Addition) to SLR1|__Key__ __Concepts__<br> - SLR Assignments<br>__Keywords__<br> - slr

[.]:.
[advanced_config/]:advanced_config/
[kernel_swap/]:kernel_swap/
[multiple_devices/]:multiple_devices/
[multiple_process/]:multiple_process/
[slr_assign/]:slr_assign/
