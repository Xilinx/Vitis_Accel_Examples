Performance Examples
==================================
List of examples that cover performance related aspect.

**Examples Table :**

.. list-table:: 
  :header-rows: 1

  * - **Example**
    - **Description**
    - **Key Concepts/Keywords**
  * - `axi_burst_performance <axi_burst_performance>`_
    - This is an AXI Burst Performance check design. It measures the time it takes to write a buffer into DDR or read a buffer from DDR. The example contains 2 sets of 6 kernels each: each set having a different data width and each kernel having a different burst_length and num_outstanding parameters to compare the impact of these parameters on effective throughput.
    - 

  * - `hbm_bandwidth <hbm_bandwidth>`_
    - This is a HBM bandwidth check design. Design contains 3 compute units of a kernel which has access to all HBM pseudo-channels (0:31). Host application allocate buffer into all HBM banks and run these 3 compute units concurrently and measure the overall bandwidth between Kernel and HBM Memory.
    - 

  * - `hbm_bandwidth_pseudo_random <hbm_bandwidth_pseudo_random>`_
    - This is a HBM bandwidth example using a pseudo random 1024 bit data access pattern to mimic Ethereum Ethash workloads. The design contains 3 compute units of a kernel, reading 1024 bits from a pseudo random address in each of 2 pseudo channels and writing the results of a simple mathematical operation to a pseudo random address in 2 other pseudo channels. To maximize bandwidth the pseudo channels are used in  P2P like configuration - See https://developer.xilinx.com/en/articles/maximizing-memory-bandwidth-with-vitis-and-xilinx-ultrascale-hbm-devices.html for more information on HBM memory access configurations. The host application allocates buffers in 12  HBM banks and runs the compute units concurrently to measure the overall bandwidth between kernel and HBM Memory.
    - 
      **Key Concepts**

      * `High Bandwidth Memory <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/HBM-Configuration-and-Use>`__
      * `Multiple HBM Pseudo-channels <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/HBM-Configuration-and-Use>`__
      * Random Memory Access

      * Linear Feedback Shift Register

      **Keywords**

      * `HBM <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/HBM-Configuration-and-Use>`__
      * `XCL_MEM_TOPOLOGY <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Assigning-DDR-Bank-in-Host-Code>`__
      * `cl_mem_ext_ptr_t <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Assigning-DDR-Bank-in-Host-Code>`__

  * - `host_global_bandwidth <host_global_bandwidth>`_
    - Host to global memory bandwidth test
    - 

  * - `host_memory_bandwidth <host_memory_bandwidth>`_
    - This is host memory bandwidth example.
    - 
      **Key Concepts**

      * `host memory <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Best-Practices-for-Host-Programming>`__
      * bandwidth

      * address translation unit

      **Keywords**

      * `XCL_MEM_EXT_HOST_ONLY <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Assigning-DDR-Bank-in-Host-Code>`__
      * `HOST[0] <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Mapping-Kernel-Ports-to-Memory>`__

  * - `host_memory_bandwidth_xrt <host_memory_bandwidth_xrt>`_
    - This is host memory bandwidth example to describe host memory and kernel bandwidth test using native xrt api's.
    - 
      **Key Concepts**

      * `host memory <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Best-Practices-for-Host-Programming>`__
      * bandwidth

      * address translation unit

      **Keywords**

      * host_only
      * `HOST[0] <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Mapping-Kernel-Ports-to-Memory>`__

  * - `iops_test_xrt <iops_test_xrt>`_
    - This is simple test design to measure Input/Output Operations per second. In this design, a simple kernel is enqueued many times and measuring overall IOPS using XRT native api's.
    - 
      **Key Concepts**

      * Input/Output Operations per second


  * - `kernel_global_bandwidth <kernel_global_bandwidth>`_
    - Bandwidth test of global to local memory.
    - 

  * - `p2p_fpga2fpga_bandwidth <p2p_fpga2fpga_bandwidth>`_
    - This is simple example to explain performance bandwidth for P2P transfer between two FPGA devices.
    - 
      **Key Concepts**

      * `P2P <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Special-Data-Transfer-Models>`__
      * Multi-FPGA Execution

      * XDMA

      **Keywords**

      * `XCL_MEM_EXT_P2P_BUFFER <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Assigning-DDR-Bank-in-Host-Code>`__


