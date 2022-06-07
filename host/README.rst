Host Examples
==================================
OpenCL host code for optimized interfacing with Xilinx Devices.

**Examples Table :**

.. list-table:: 
  :header-rows: 1

  * - **Example**
    - **Description**
    - **Key Concepts/Keywords**
  * - `concurrent_kernel_execution <concurrent_kernel_execution>`_
    - This example will demonstrate how to use multiple and out of order command queues to simultaneously execute multiple kernels on an FPGA.
    - **Key Concepts**

      * `Concurrent execution <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Task-Parallelism>`__
      * `Out of Order Command Queues <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Single-Out-of-Order-Command-Queue>`__
      * `Multiple Command Queues <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Multiple-In-Order-Command-Queues>`__
      **Keywords**

      * `CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Single-Out-of-Order-Command-Queue>`__
      * `setCallback <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Single-Out-of-Order-Command-Queue>`__

  * - `copy_buffer <copy_buffer>`_
    - This Copy Buffer example demonstrate how one buffer can be copied from another buffer.
    - **Key Concepts**

      * Copy Buffer

      **Keywords**

      * `cl::CommandQueue <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Command-Queues>`__
      * enqueueCopyBuffer
      * `enqueueWriteBuffer <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Buffer-Creation-and-Data-Transfer>`__
      * `enqueueReadBuffer <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Buffer-Creation-and-Data-Transfer>`__
      * `enqueueMigrateMemObjects <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Buffer-Creation-and-Data-Transfer>`__

  * - `data_transfer <data_transfer>`_
    - This example illustrates several ways to use the OpenCL API to transfer data to and from the FPGA
    - **Key Concepts**

      * `OpenCL Host APIs <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/OpenCL-Programming>`__
      * `Data Transfer <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Buffer-Creation-and-Data-Transfer>`__
      * `Write Buffers <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Buffer-Creation-and-Data-Transfer>`__
      * `Read Buffers <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Buffer-Creation-and-Data-Transfer>`__
      * `Map Buffers <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Buffer-Creation-and-Data-Transfer>`__
      * Async Memcpy

      **Keywords**

      * `enqueueWriteBuffer <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Buffer-Creation-and-Data-Transfer>`__
      * `enqueueReadBuffer <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Buffer-Creation-and-Data-Transfer>`__
      * `enqueueMapBuffer <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Buffer-Creation-and-Data-Transfer>`__
      * enqueueUnmapMemObject
      * `enqueueMigrateMemObjects <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Buffer-Creation-and-Data-Transfer>`__

  * - `debug_profile <debug_profile>`_
    - This is simple example of vector addition and printing profile data (wall clock time taken between start and stop). It also dump a waveform file which can be reloaded to vivado to see the waveform. Run command 'vivado -source ./scripts/open_waveform.tcl -tclargs <device_name>-<kernel_name>.<target>.<device_name>.wdb' to launch waveform viewer. User can also update batch to gui in xrt.ini file to see the live waveform while running application. The example also demonstrates the use of hls::print to print a format string/int/double argument to standard output, and to the simulation log in cosim and HW_EMU.
    - **Key Concepts**

      * `Use of Profile API <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Profiling-the-Application>`__
      * `Waveform Dumping and loading <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Waveform-View-and-Live-Waveform-Viewer>`__
      **Keywords**

      * `debug_mode=gui/batch <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Enable-Waveform-Debugging-with-the-Vitis-Compiler-Command>`__
      * `user_range <https://docs.xilinx.com/r/2021.1-English/ug1393-vitis-application-acceleration/Profiling-of-C-Code?tocId=Mr4opDBD1mYmK4fSMoQH5g>`__
      * `user_event <https://docs.xilinx.com/r/2021.1-English/ug1393-vitis-application-acceleration/Profiling-of-C-Code?tocId=Mr4opDBD1mYmK4fSMoQH5g>`__
      * hls::print

  * - `device_only_buffer <device_only_buffer>`_
    - This example will demonstrate how to create buffers in global memory which are not mapped to host. The device only memory allocation is done through the host code. The kernel can read data from device memory and write result to device memory.
    - **Key Concepts**

      * Device only buffer

      **Keywords**

      * CL_MEM_HOST_NO_ACCESS

  * - `device_query <device_query>`_
    - This Example prints the OpenCL properties of the platform and its devices using OpenCL CPP APIs. It also displays the limits and capabilities of the hardware.
    - **Key Concepts**

      * OpenCL API

      * Querying device properties


  * - `errors <errors>`_
    - This example discuss the different reasons for errors in OpenCL and how to handle them at runtime.
    - **Key Concepts**

      * OpenCL API

      * Error handling

      **Keywords**

      * CL_SUCCESS
      * CL_DEVICE_NOT_FOUND
      * CL_DEVICE_NOT_AVAILABLE

  * - `errors_cpp <errors_cpp>`_
    - This example discuss the different reasons for errors in OpenCL C++ and how to handle them at runtime.
    - **Key Concepts**

      * OpenCL Host API

      * Error handling

      **Keywords**

      * CL_SUCCESS
      * CL_DEVICE_NOT_FOUND
      * CL_DEVICE_NOT_AVAILABLE
      * CL_INVALID_VALUE
      * CL_INVALID_KERNEL_NAME
      * CL_INVALID_BUFFER_SIZE

  * - `hbm_large_buffers <hbm_large_buffers>`_
    - This is a simple example of vector addition to describe how HBM pseudo-channels can be grouped to handle buffers larger than 256 MB.
    - **Key Concepts**

      * `High Bandwidth Memory <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/HBM-Configuration-and-Use>`__
      * Multiple HBM Pseudo-channel Groups

      **Keywords**

      * `HBM <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/HBM-Configuration-and-Use>`__

  * - `hbm_rama_ip <hbm_rama_ip>`_
    - This is host application to test HBM interface bandwidth for buffers > 256 MB with pseudo random 1024 bit data access pattern, mimicking Ethereum Ethash workloads. Design contains 4 compute units of Kernel, 2 with and 2 without RAMA IP. Each compute unit reads 1024 bits from a pseudo random address in each of 2 pseudo channel groups and writes the results of a simple mathematical operation to a pseudo random address in 2 other pseudo channel groups. Each buffer is 1 GB large requiring 4 HBM banks. Since the first 2 CUs requires 4 buffers each and are then used again by the other 2 CUs, the .cfg file is allocating the buffers to all the 32 HBM banks.  The host application runs the compute units concurrently to measure the overall bandwidth between kernel and HBM Memory.
    - **Key Concepts**

      * `High Bandwidth Memory <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/HBM-Configuration-and-Use>`__
      * `Multiple HBM Pseudo-channels <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/HBM-Configuration-and-Use>`__
      * Random Memory Access

      * Linear Feedback Shift Register

      * `RAMA IP <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Random-Access-and-the-RAMA-IP>`__
      **Keywords**

      * `HBM <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/HBM-Configuration-and-Use>`__
      * `ra_master_interface <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Random-Access-and-the-RAMA-IP>`__

  * - `hbm_simple <hbm_simple>`_
    - This is a simple example of vector addition to describe how to use HLS kernels with HBM (High Bandwidth Memory) for achieving high throughput.
    - **Key Concepts**

      * `High Bandwidth Memory <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/HBM-Configuration-and-Use>`__
      * Multiple HBM pseudo-channels

      **Keywords**

      * `HBM <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/HBM-Configuration-and-Use>`__
      * `XCL_MEM_TOPOLOGY <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Assigning-DDR-Bank-in-Host-Code>`__
      * `cl_mem_ext_ptr_t <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Assigning-DDR-Bank-in-Host-Code>`__
      * `trace_memory <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/profile-Options>`__
      * `trace_buffer_size <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/xrt.ini-File>`__
      * `opencl_trace <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/xrt.ini-File>`__

  * - `host_memory_copy_buffer <host_memory_copy_buffer>`_
    - This is simple host memory example to describe how host-only memory can be copied to device-only memory and vice-versa.
    - **Key Concepts**

      * `host memory <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Best-Practices-for-Host-Programming>`__
      **Keywords**

      * XCL_MEM_EXT_HOST_ONLY
      * CL_MEM_HOST_NO_ACCESS
      * enqueueCopyBuffer

  * - `host_memory_copy_kernel <host_memory_copy_kernel>`_
    - This is a Host Memory Example to describe how data can be copied between host-only buffer and device-only buffer using User Copy Kernel.
    - **Key Concepts**

      * `host memory <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Best-Practices-for-Host-Programming>`__
      **Keywords**

      * XCL_MEM_EXT_HOST_ONLY
      * CL_MEM_HOST_NO_ACCESS
      * `enqueueMapBuffer <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Buffer-Creation-and-Data-Transfer>`__

  * - `host_memory_simple <host_memory_simple>`_
    - This is simple host memory example to describe how a user kernel can access the host memory. The host memory allocation is done through the host code. The kernel reads data from host memory and writes result to host memory.
    - **Key Concepts**

      * `host memory <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Best-Practices-for-Host-Programming>`__
      * address translation unit

      **Keywords**

      * XCL_MEM_EXT_HOST_ONLY
      * `HOST[0] <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Mapping-Kernel-Ports-to-Memory>`__

  * - `iops_test <iops_test>`_
    - This is simple test design to measure Input/Output Operations per second. In this design, a simple kernel is enqueued many times and measuring overall IOPS.
    - **Key Concepts**

      * Input/Output Operations per second

      **Keywords**

      * `CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Single-Out-of-Order-Command-Queue>`__

  * - `mult_compute_units <mult_compute_units>`_
    - This is simple Example of Multiple Compute units to showcase how a single kernel can be instantiated into Multiple compute units. Host code will show how to use multiple compute units and run them concurrently.
    - **Key Concepts**

      * `Multiple Compute Units <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Symmetrical-and-Asymmetrical-Compute-Units>`__
      **Keywords**

      * `nk <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/connectivity-Options>`__

  * - `multiple_cus_asymmetrical <multiple_cus_asymmetrical>`_
    - This is simple example of vector addition to demonstrate how to connect each compute unit to different banks and how to use these compute units in host applications
    - **Key Concepts**

      * `Multiple Compute Units <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Symmetrical-and-Asymmetrical-Compute-Units>`__
      * `Task Level Parallelism <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Task-Parallelism>`__

  * - `overlap <overlap>`_
    - This examples demonstrates techniques that allow user to overlap Host(CPU) and FPGA computation in an application. It will cover asynchronous operations and event object.
    - **Key Concepts**

      * OpenCL Host API

      * `Synchronize Host and FPGA <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Event-Synchronization>`__
      * `Asynchronous Processing <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Event-Synchronization>`__
      * `Events <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Overlapping-Data-Transfers-with-Kernel-Computation>`__
      * `Asynchronous memcpy <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Event-Synchronization>`__
      **Keywords**

      * `cl_event <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Event-Synchronization>`__
      * `cl::CommandQueue <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Command-Queues>`__
      * `CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Single-Out-of-Order-Command-Queue>`__
      * `enqueueMigrateMemObjects <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Buffer-Creation-and-Data-Transfer>`__

  * - `p2p_bandwidth <p2p_bandwidth>`_
    - This is simple example to test data transfer between SSD and FPGA.
    - **Key Concepts**

      * `P2P <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/p2p>`__
      * SmartSSD

      * XDMA

      **Keywords**

      * XCL_MEM_EXT_P2P_BUFFER
      * pread
      * pwrite

  * - `p2p_fpga2fpga <p2p_fpga2fpga>`_
    - This is simple example to explain P2P transfer between two FPGA devices.
    - **Key Concepts**

      * `P2P <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/p2p>`__
      * Multi-FPGA Execution

      * XDMA

      **Keywords**

      * XCL_MEM_EXT_P2P_BUFFER

  * - `p2p_overlap_bandwidth <p2p_overlap_bandwidth>`_
    - This is simple example to test Synchronous and Asyncronous data transfer between SSD and FPGA.
    - **Key Concepts**

      * `P2P <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/p2p>`__
      * SmartSSD

      * XDMA

      **Keywords**

      * XCL_MEM_EXT_P2P_BUFFER
      * pread
      * pwrite

  * - `p2p_simple <p2p_simple>`_
    - This is simple example of vector increment to describe P2P between FPGA and NVMe SSD.
    - **Key Concepts**

      * `P2P <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/p2p>`__
      * NVMe SSD

      * SmartSSD

      **Keywords**

      * XCL_MEM_EXT_P2P_BUFFER
      * pread
      * pwrite
      * `O_DIRECT <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Special-Data-Transfer-Models>`__
      * O_RDWR

  * - `streaming_free_running_k2k <streaming_free_running_k2k>`_
    - This is simple example which demonstrate how to use and configure a free running kernel.
    - **Key Concepts**

      * `Free Running Kernel <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Free-Running-Kernel>`__
      **Keywords**

      * `ap_ctrl_none <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Block-Level-Control-Protocols>`__
      * `stream_connect <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Specifying-Streaming-Connections-between-Compute-Units>`__

  * - `streaming_k2k_mm <streaming_k2k_mm>`_
    - This is a simple kernel to kernel streaming Vector Add and Vector Multiply C Kernel design with 2 memory mapped input to kernel 1, 1 Stream output from kernel 1 to input of kernel 2, 1 memory mapped input to kernel 2, and 1 memory mapped output that demonstrates on how to process a stream of data for computation between two kernels. This design also illustrates how to set FIFO depth for AXIS connections i.e. for the stream connecting the two kernels
    - **Key Concepts**

      * `Read/Write Stream <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Host-Coding-for-Free-Running-Kernels>`__
      * `Create/Release Stream <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Host-Coding-for-Free-Running-Kernels>`__
      * `AXIS FIFO depth <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Specifying-Compiler-Created-FIFO-Depth>`__
      **Keywords**

      * `stream_connect <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Specifying-Streaming-Connections-between-Compute-Units>`__


