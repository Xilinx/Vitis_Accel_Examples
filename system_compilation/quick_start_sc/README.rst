Quick Start System Compiler
===========================

This is quick start example based on a video filtering usecase to describe system compiler usage in Vitis Environment.

**KEY CONCEPTS:** `System Compiler <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Using-Vitis-System-Compilation-Mode>`__

**KEYWORDS:** `create_bufpool <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Quick-Start-Example>`__, `alloc_buff <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Quick-Start-Example>`__, `compute <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/The-compute-API>`__, `send_while <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Interface-Data-Types>`__, `receive_all_in_order <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/VPP_ACC-Class-API>`__, `get_buf <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/VPP_ACC-Class-API>`__, `DATA_COPY <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Guidance-Macros>`__, `SYS_PORT <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Guidance-Macros>`__, `SYS_PORT_PFM <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Guidance-Macros>`__, `ACCESS_PATTERN <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Guidance-Macros>`__

.. raw:: html

 <details>

.. raw:: html

 <summary> 

 <b>EXCLUDED PLATFORMS:</b>

.. raw:: html

 </summary>
|
..

 - Alveo U25 SmartNIC
 - All NoDMA Platforms, i.e u50 nodma etc
 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - All Versal Platforms, i.e vck190 etc
 - AWS VU9P F1

.. raw:: html

 </details>

.. raw:: html

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/coefficients.h
   src/common.h
   src/conv_filter_acc.cpp
   src/conv_filter_acc.hpp
   src/conv_filter_acc_wrapper.cpp
   src/conv_filter_acc_wrapper.hpp
   src/conv_filter_cpu.cpp
   src/conv_filter_cpu.hpp
   src/host_randomized.cpp
   src/types.hpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./host.exe

DETAILS
-------

The convolutional filter is based on a video filtering use case. An image filter is applied to all the channels of a video namely YUV on per frame basis. The host generates a stream of images for each color channels which are transferred to device for processing. The RGB video has three parallel channels providing coarse-grain parallelism which allows to process three color channels independently using a separate compute unit per channel. So essentially the problem is to design an FPGA based video filter where each video channel can be processed in parallel by a separate compute units. The convolutional filter applied to each channel is same so it is possible that we can build a single kernel and use three instances of same kernel(compute units) for acceleration. 

- VPP_ACC Interface Class
The VPP_ACC class provides the basic API's. All accelerators should be written as a class derived from the VPP_ACC base class. In user code, every class that inherits from the VPP_ACC class should intend to be an accelerator that compiles to hardware. Specifically the child class should provide a function named "compute" that is the software entry point to the compiled hardware accelerator.  Every child accelerator class should have a unique name. 

The VPP_ACC class provides the following API:

- create_bufpool(<direction>)
For each non-scalar compute() argument, input or output of the kernel, the user is required to create this object, which is a pool of buffers that is used to transfer data between host and the device. The create_bufpool object must be used within the send_while thread to allocate memory for input data arguments of compute(). These bufpool objects have to be created in the scope that is global to both send and receive threads. This API also takes an argument which is of type "vpp::input  or vpp::output" to specify if this buffer is input or output with respect to CU.

- bool send_while (<lambda function>)
The send_while API provides a way to wrap user defined C++ lambda function. The defines a lambda function that allocates buffers on the device and calls "compute" function. The system compiler will generate a separate thread that will call compute() in a loop, to execute the kernel. This thread also manages data transfers to the cluster of compute units specified in the class derivation.  The lambda function must return a boolean value, which should be true to continue execution of the loop and thread. The send_while loop exits when the lambda function returns false, and in turn the parent C-thread also completes and terminates.

- alloc_buf(<bufpool>, size)
Allocates buffer given the handle to buffer pool and size in number of bytes of data.This is required for each argument of compute, except when it is a scalar (non-pointer and non-array native C dataype supported by System Compiler).The size argument need not to be a constant, and system compiler allows dynamic allocation of memory at run time. This API must be used only in the scope of the send_while API body.

- Compute function call
Once buffer are allocated for inputs and outputs, and the input data is available for all non-scalar arguments the compute can be triggered with a call to compute(). This must be done within the send_while lamda function body.

- receive_all_in_order (<lambda_function>)
When multiple calls are made to the same accelerated function which have more than one compute units (instances). Then depending on the nature of kernel the output data from these compute units may arrive out in a different order compared to the order in which calls were made. The receive_all_in_order API is provided to make sure that the output data is received in same order as the compute calls, and independent of the order in which compute different units finish. Like the send_while, this function also executes a lambda function body in a loop within a separate C-thread which automatically exits when send_while thread completes.

- get_buf(<bufpool>)
Returns a host side pointer to read output data given the handle to buffer bool for current iteration. This API must be used only in the scope of the receive thread/API body.

- join()
For every send_while and receive API call which are run in separate threads the user is required to make join call, which essentially creates a join for these C-threads.

- ACCESS_PATTERN(<port>, <pattern>)
This macro is used to specify the data access pattern on a given CU port/memory interface. Based on the access pattern system compiler can generate an appropriate data mover plugin for the kernel. Where <port> is the kernel port and  <pattern> can be sequential/random.

- DATA_COPY(<port>, <port>[<num>])
Specifies the amount of data to be copied by the data mover at runtime between host and device for CU processing for given port. The "port" needs to be replaced by port name ( kernel function argument name). The "num" specifies the number of data elements to be transferred the size of data. The num can be a constant or it can be itself an expression in terms of scalar kernel arguments.

- SYS_PORT(<port>, <memBank>)
Specifies which memory bank to use for a given port connection (identical for all CU's). The "memBank" specifies the bank name such as DDR[0] etc and "port" is the CU argument name.

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.