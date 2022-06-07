HLS C/C++ Kernel Examples
==================================
This section contains HLS C/C++ Kernel Examples. The main purpose of C++ kernels is to showcase the benefits and usage of HLS pragmas and optimization directives in Vitis flow.

**Examples Table :**

.. list-table:: 
  :header-rows: 1

  * - **Example**
    - **Description**
    - **Key Concepts/Keywords**
  * - `array_partition <array_partition>`_
    - This is a simple example of matrix multiplication (Row x Col) to demonstrate how to achieve better performance by array partitioning, using HLS kernel in Vitis Environment.
    - **Key Concepts**

      * `Kernel Optimization <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Kernel-Optimization>`__
      * `HLS C Kernel <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Interfaces>`__
      * `Array Partition <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-array_partition>`__
      **Keywords**

      * `#pragma HLS ARRAY_PARTITION <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-array_partition>`__
      * `complete <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-array_partition>`__

  * - `bind_op_storage <bind_op_storage>`_
    - This is simple example of vector addition to describe how to use BIND OP and STORAGE for better implementation style.
    - **Key Concepts**

      * `BIND OP <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-bind_op>`__
      * `BIND STORAGE <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-bind_storage>`__
      **Keywords**

      * `BIND_OP <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-bind_op>`__
      * `BIND_STORAGE <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-bind_storage>`__
      * `impl <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-bind_op>`__
      * `op <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-bind_op>`__
      * `type <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-bind_op>`__
      * `latency <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-bind_op>`__

  * - `burst_rw <burst_rw>`_
    - This is simple example of using AXI4-master interface for burst read and write
    - **Key Concepts**

      * `burst access <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/AXI-Burst-Transfers>`__
      **Keywords**

      * `memcpy <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Defining-Interfaces>`__
      * `max_read_burst_length <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/AXI-Burst-Transfers>`__
      * `max_write_burst_length <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/AXI-Burst-Transfers>`__

  * - `critical_path <critical_path>`_
    - This example shows a normal coding style which could lead to critical path issue and design will give degraded timing.  Example also contains better coding style which can improve design timing.
    - **Key Concepts**

      * Critical Path handling

      * Improve Timing


  * - `custom_datatype <custom_datatype>`_
    - This is simple example of RGB to HSV conversion to demonstrate Custom DATA Type usages in C Based Kernel. Xilinx HLS Compiler Supports Custom Data Type to use for operation as well as Memory Interface between Kernel and Global Memory.
    - **Key Concepts**

      * `Custom Datatype <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Composite-Data-Types>`__
      **Keywords**

      * `struct <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Structs>`__
      * `#pragma HLS LOOP_TRIPCOUNT <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-loop_tripcount>`__

  * - `dataflow_stream <dataflow_stream>`_
    - This is simple example of vector addition to demonstrate Dataflow functionality of HLS. HLS Dataflow allows user to schedule multiple task together to achieve higher throughput.
    - **Key Concepts**

      * `Task Level Parallelism <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Task-Parallelism>`__
      **Keywords**

      * `dataflow <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Optimization-Techniques-in-Vitis-HLS>`__
      * `hls::stream <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/HLS-Stream-Library>`__

  * - `dataflow_stream_array <dataflow_stream_array>`_
    - This is simple example of Multiple Stages Vector Addition to demonstrate Array of Stream usage in HLS C Kernel Code.
    - **Key Concepts**

      * Array of Stream

      **Keywords**

      * `dataflow <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Optimization-Techniques-in-Vitis-HLS>`__
      * `hls::stream <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/HLS-Stream-Library>`__

  * - `dependence_inter <dependence_inter>`_
    - This Example demonstrates the HLS pragma 'DEPENDENCE'.Using 'DEPENDENCE' pragma, user can provide additional dependency details to the compiler by specifying if the dependency in consecutive loop iterations on buffer is true/false, which allows the compiler to perform unrolling/pipelining to get better performance.
    - **Key Concepts**

      * `Inter Dependence <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Managing-Pipeline-Dependencies>`__
      **Keywords**

      * `DEPENDENCE <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-dependence>`__
      * `inter <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-dependence>`__
      * `WAR <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-dependence>`__

  * - `gmem_2banks <gmem_2banks>`_
    - This example of 2ddr is to demonstrate how to use multiple ddr and create buffers in each DDR.
    - **Key Concepts**

      * `Multiple Banks <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Using-Multiple-DDR-Banks>`__
      **Keywords**

      * `m_axi_auto_max_ports <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/MAXI>`__
      * `sp <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/connectivity-Options>`__
      * `connectivity <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/connectivity-Options>`__

  * - `kernel_chain <kernel_chain>`_
    - This is a kernel containing the cascaded Matrix Multiplication using dataflow. ap_ctrl_chain is enabled for this kernel to showcase how multiple enqueue of Kernel calls can be overlapped to give higher performance. ap_ctrl_chain allow kernel to start processing of next kernel operation before completing the current kernel operation.
    - **Key Concepts**

      * `ap_ctrl_chain <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Block-Level-Control-Protocols>`__
      * PLRAM


  * - `lmem_2rw <lmem_2rw>`_
    - This is simple example of vector addition to demonstrate how to utilize both ports of Local Memory.
    - **Key Concepts**

      * `Kernel Optimization <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Kernel-Optimization>`__
      * `2port BRAM Utilization <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Port-Level-I/O-Memory-Interface-Protocol>`__
      * two read/write Local Memory

      **Keywords**

      * `#pragma HLS UNROLL FACTOR=2 <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-unroll>`__

  * - `loop_pipeline <loop_pipeline>`_
    - This example demonstrates how loop pipelining can be used to improve the performance of a kernel.
    - **Key Concepts**

      * `Kernel Optimization <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Kernel-Optimization>`__
      * `Loop Pipelining <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Function-and-Loop-Pipelining>`__
      **Keywords**

      * `pragma HLS PIPELINE <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-pipeline>`__

  * - `loop_reorder <loop_reorder>`_
    - This is a simple example of matrix multiplication (Row x Col) to demonstrate how to achieve better pipeline II factor by loop reordering.
    - **Key Concepts**

      * `Kernel Optimization <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Kernel-Optimization>`__
      * Loop reorder to improve II

      **Keywords**

      * `#pragma HLS ARRAY_PARTITION <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-array_partition>`__

  * - `partition_cyclicblock <partition_cyclicblock>`_
    - This example shows how to use array block and cyclic partitioning to improve performance of a kernel
    - **Key Concepts**

      * `Kernel Optimization <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Kernel-Optimization>`__
      * Array Partitioning

      * `Block Partition <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-array_partition>`__
      * `Cyclic Partition <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-array_partition>`__
      **Keywords**

      * `#pragma HLS ARRAY_PARTITION <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-array_partition>`__
      * `cyclic <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-array_partition>`__
      * `block <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-array_partition>`__
      * `factor <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-array_partition>`__
      * `dim <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-array_partition>`__

  * - `plram_access <plram_access>`_
    - This example shows the usage of PLRAM and how to use it with simple matrix multiplication (Row x Col).
    - **Key Concepts**

      * Vitis Memory Hierarchy

      * `PLRAMs <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/PLRAM-Configuration-and-Use>`__
      **Keywords**

      * `PLRAM <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/PLRAM-Configuration-and-Use>`__

  * - `port_width_widening <port_width_widening>`_
    - This example shows how HLS introduces the capability of resizing the port width of the kernel interface ports for better resource utilization maintaining the performance.
    - **Key Concepts**

      * `Interface port width auto widening <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Automatic-Port-Width-Resizing>`__
      **Keywords**

      * m_axi_max_widen_bitwidth 

  * - `shift_register <shift_register>`_
    - This example demonstrates how to shift values in registers in each clock cycle
    - **Key Concepts**

      * `Kernel Optimization <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Kernel-Optimization>`__
      * `Shift Register <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Inferring-Shift-Registers>`__
      * `FIR <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Inferring-Shift-Registers>`__
      **Keywords**

      * `#pragma HLS ARRAY_PARTITION <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-array_partition>`__

  * - `simple_vadd <simple_vadd>`_
    - This is a simple example of vector addition.The purpose of this code is to introduce the user to application development in the Vitis tools.
    - 
  * - `systolic_array <systolic_array>`_
    - This is a simple example of matrix multiplication (Row x Col) to help developers learn systolic array based algorithm design. Note : Systolic array based algorithm design is well suited for FPGA.
    - 
  * - `wide_mem_rw <wide_mem_rw>`_
    - This is simple example of vector addition to demonstrate Wide Memory Access using ap_uint<512> data type. Based on input argument type, V++ compiler will figure our the memory datawidth between Global Memory and Kernel. For this example, ap_uint<512> datatype is used, so Memory datawidth will be 16 x (integer bit size) = 16 x 32 = 512 bit.
    - **Key Concepts**

      * `Kernel to DDR <https://docs.xilinx.com/r/2021.2-English/ug1393-vitis-application-acceleration/Mapping-Kernel-Ports-to-Memory>`__
      * `wide memory access <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/AXI-Burst-Transfers>`__
      * `burst read and write <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/AXI-Burst-Transfers>`__
      **Keywords**

      * `ap_uint<> <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/C-Arbitrary-Precision-Integer-Types>`__
      * `ap_int.h <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/C-Arbitrary-Precision-Integer-Types>`__


