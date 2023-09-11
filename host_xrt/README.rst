Native XRT Examples
==================================
XRT Native API's for optimized interfacing with Xilinx Devices.

**Examples Table :**

.. list-table:: 
  :header-rows: 1

  * - **Example**
    - **Description**
    - **Key Concepts/Keywords**
  * - `asynchronous_xrt <asynchronous_xrt>`_
    - This is simple example which showcases asynchronous programming mechanism through the user-defined queues.
    - 
      **Key Concepts**

      * `XRT Native API <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Setting-Up-XRT-Managed-Kernels-and-Kernel-Arguments>`__
      * `Asynchronous Programming <https://xilinx.github.io/XRT/2023.1/html/xrt_native_apis.html?highlight=queue#asynchornous-programming-with-xrt-experimental>`__
      **Keywords**

      * `xrt::queue <https://xilinx.github.io/XRT/2023.1/html/xrt_native_apis.html?highlight=queue#executing-multiple-tasks-through-queue>`__
      * `enqueue <https://xilinx.github.io/XRT/2023.1/html/xrt_native_apis.html?highlight=queue#executing-multiple-tasks-through-queue>`__
      * `wait() <https://xilinx.github.io/XRT/2023.1/html/xrt_native_apis.html?highlight=queue#executing-multiple-tasks-through-queue>`__

  * - `copy_buffer_xrt <copy_buffer_xrt>`_
    - This Copy Buffer example demonstrate how one buffer can be copied from another buffer.
    - 
      **Key Concepts**

      * Copy Buffer

      **Keywords**

      * copy

  * - `data_transfer_xrt <data_transfer_xrt>`_
    - This example illustrates transferring of data to and from the FPGA using xrt native api's
    - 
      **Key Concepts**

      * `XRT Native API <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Setting-Up-XRT-Managed-Kernels-and-Kernel-Arguments>`__
      * `Data Transfer <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Buffer-Creation-and-Data-Transfer>`__
      * `Write Buffers <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Buffer-Creation-and-Data-Transfer>`__
      * `Read Buffers <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Buffer-Creation-and-Data-Transfer>`__
      **Keywords**

      * `xrt::bo::write <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Transferring-Data-between-Software-and-PL-Kernels>`__
      * `xrt::bo::read <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Transferring-Data-between-Software-and-PL-Kernels>`__

  * - `device_only_buffer_xrt <device_only_buffer_xrt>`_
    - This example will demonstrate how to create buffers in global memory which are not mapped to host.
    - 
      **Key Concepts**

      * Device only buffer

      **Keywords**

      * xrt::bo::flags::device_only

  * - `device_query_xrt <device_query_xrt>`_
    - This Example prints device properties using XRT Native APIs. It also displays the limits and capabilities of the hardware.
    - 
      **Key Concepts**

      * Querying device properties

      **Keywords**

      * `xrt::device <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Specifying-the-Device-ID-and-Loading-the-XCLBIN>`__

  * - `hbm_simple_xrt <hbm_simple_xrt>`_
    - This is a simple example of vector addition to describe how to use HLS kernels with HBM (High Bandwidth Memory) for achieving high throughput using xrt native api's.
    - 
      **Key Concepts**

      * `High Bandwidth Memory <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/HBM-Configuration-and-Use>`__
      * Multiple HBM Banks

      **Keywords**

      * `HBM <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/HBM-Configuration-and-Use>`__

  * - `hello_world_xrt <hello_world_xrt>`_
    - This is simple example of vector addition to describe the usage of XRT Native API's. The kernel uses HLS Dataflow which allows the user to schedule multiple task together to achieve higher throughput.
    - 
      **Key Concepts**

      * `XRT Native API <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Setting-Up-XRT-Managed-Kernels-and-Kernel-Arguments>`__
      * `Task Level Parallelism <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Data-driven-Task-level-Parallelism>`__
      **Keywords**

      * `xrt::device <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Specifying-the-Device-ID-and-Loading-the-XCLBIN>`__
      * load_xclbin
      * `xrt::bo <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Writing-Host-Applications-with-XRT-API>`__
      * `xrt::kernel <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Setting-Up-XRT-Managed-Kernels-and-Kernel-Arguments>`__
      * `map <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Transferring-Data-between-Software-and-PL-Kernels>`__
      * `sync <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Transferring-Data-between-Software-and-PL-Kernels>`__
      * `XCL_BO_SYNC_BO_TO_DEVICE <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Writing-Host-Applications-with-XRT-API>`__
      * `XCL_BO_SYNC_BO_FROM_DEVICE <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Writing-Host-Applications-with-XRT-API>`__
      * `gmem <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Mapping-Kernel-Ports-to-Memory>`__
      * `#pragma HLS INTERFACE <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/HLS-Pragmas>`__
      * `dataflow <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Dataflow>`__
      * `hls::stream <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/HLS-Stream-Library>`__

  * - `host_memory_copy_buffer_xrt <host_memory_copy_buffer_xrt>`_
    - This is simple host memory example to describe how host-only memory can be copied to device-only memory and vice-versa using XRT Native API's.
    - 
      **Key Concepts**

      * `host memory <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Best-Practices-for-Host-Programming>`__
      **Keywords**

      * host_only
      * device_only
      * `HOST[0] <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Mapping-Kernel-Ports-to-Memory>`__

  * - `host_memory_copy_kernel_xrt <host_memory_copy_kernel_xrt>`_
    - This is a Host Memory Example to describe how data can be copied using xrt native api's between host-only buffer and device-only buffer using User Copy Kernel.
    - 
      **Key Concepts**

      * `host memory <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Best-Practices-for-Host-Programming>`__
      **Keywords**

      * host_only
      * device_only

  * - `host_memory_simple_xrt <host_memory_simple_xrt>`_
    - This is simple host memory example to describe how a user kernel can access the host memory using xrt native api's.
    - 
      **Key Concepts**

      * `host memory <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Best-Practices-for-Host-Programming>`__
      * address translation unit

      **Keywords**

      * host_only
      * `HOST[0] <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Mapping-Kernel-Ports-to-Memory>`__

  * - `mailbox_auto_restart_xrt <mailbox_auto_restart_xrt>`_
    - This is a simple example demonstrating the Mailbox and Auto-restart features for writing registers to and reading registers from a never-ending kernel.
    - 
      **Key Concepts**

      * `Mailbox <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Using-the-Mailbox>`__
      * `Auto-restart <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Auto-Restarting-Mode>`__
      **Keywords**

      * `set_arg <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Working-with-XRT-Managed-Kernels>`__
      * get_arg
      * write()
      * read()
      * STABLE

  * - `mult_compute_units_xrt <mult_compute_units_xrt>`_
    - This is simple Example of Multiple Compute units to showcase how a single kernel can be instantiated into Multiple compute units. Host code will show how to use multiple compute units and run them concurrently using XRT Native api's.
    - 
      **Key Concepts**

      * `Multiple compute units <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Symmetrical-and-Asymmetrical-Compute-Units>`__
      **Keywords**

      * `nk <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/connectivity-Options>`__

  * - `multiple_cus_asymmetrical_xrt <multiple_cus_asymmetrical_xrt>`_
    - This is simple example of vector addition to demonstrate how to connect each compute unit to different banks and how to use these compute units in host applications using xrt native api's.
    - 
      **Key Concepts**

      * `Multiple compute units <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Symmetrical-and-Asymmetrical-Compute-Units>`__
      * `Task Level Parallelism <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Data-driven-Task-level-Parallelism>`__

  * - `p2p_fpga2fpga_xrt <p2p_fpga2fpga_xrt>`_
    - This is simple example to explain P2P transfer between two FPGA devices using xrt native api's.
    - 
      **Key Concepts**

      * `P2P <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Special-Data-Transfer-Models>`__
      * Multi-FPGA Execution

      * XDMA

      **Keywords**

      * xrt::bo::flags::p2p

  * - `p2p_simple_xrt <p2p_simple_xrt>`_
    - This is simple example of vector increment to describe P2P between FPGA and NVMe SSD using xrt native api's.
    - 
      **Key Concepts**

      * `P2P <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Special-Data-Transfer-Models>`__
      * NVMe SSD

      * SmartSSD

      **Keywords**

      * xrt::bo::flags::p2p
      * pread
      * pwrite
      * `O_DIRECT <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Special-Data-Transfer-Models>`__
      * O_RDWR

  * - `streaming_free_running_k2k_xrt <streaming_free_running_k2k_xrt>`_
    - This is simple example which demonstrate how to use and configure a free running kernel.
    - 
      **Key Concepts**

      * `Free Running Kernel <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Free-Running-Kernel>`__
      **Keywords**

      * `ap_ctrl_none <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Block-Level-Control-Protocols>`__
      * `stream_connect <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Specifying-Streaming-Connections>`__

  * - `streaming_k2k_mm_xrt <streaming_k2k_mm_xrt>`_
    - This is a simple kernel to kernel streaming Vector Add and Vector Multiply C Kernel design with 2 memory mapped input to kernel 1, 1 Stream output from kernel 1 to input of kernel 2, 1 memory mapped input to kernel 2, and 1 memory mapped output that demonstrates on how to process a stream of data for computation between two kernels using XRT Native APIs. This design also illustrates how to set FIFO depth for AXIS connections i.e. for the stream connecting the two kernels
    - 
      **Key Concepts**

      * `Read/Write Stream <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Writing-the-Software-Application>`__
      * `Create/Release Stream <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Writing-the-Software-Application>`__
      * `AXIS FIFO depth <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Specifying-Compiler-Created-FIFO-Depth>`__
      **Keywords**

      * `stream_connect <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Specifying-Streaming-Connections>`__


