System Compiler Examples
==================================
This section contains System Compiler based Examples.

**Examples Table :**

.. list-table:: 
  :header-rows: 1

  * - **Example**
    - **Description**
    - **Key Concepts/Keywords**
  * - `debug_profile_sc <debug_profile_sc>`_
    - This simple example implements a HW pipeline of three tasks, matrix multiplication along with two different increments to the matrix values.
    - **Key Concepts**

      * `Debug <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Enabling-Profile-and-Debug-when-Linking>`__
      * `Profiling <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Profiling-Optimizing-and-Debugging-the-Application>`__
      **Keywords**

      * `PROFILE_KERNEL <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Debugging-and-Validation>`__
      * `PROFILE_PORT <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Debugging-and-Validation>`__
      * `PROFILE_OFFLOAD <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Debugging-and-Validation>`__
      * `vpp::stream<> <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Stream-connections-using-vpp-stream>`__

  * - `eth_acc_sc <eth_acc_sc>`_
    - This is simple example showcasing custom AXI-streams to PE interfaces in system compiler environment.
    - **Key Concepts**

      * `Asyncronous host interactions <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Asynchronous-Host-Control-of-Accelerator>`__
      * Custom AXI-Streams

      **Keywords**

      * `compute_async <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Asynchronous-Host-Control-of-Accelerator>`__

  * - `file_filter_sc <file_filter_sc>`_
    - This is simple example showcasing the P2P and H2C file transfers in system compiler environment.
    - **Key Concepts**

      * `P2P <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/p2p>`__
      * SSD

      * Syncronization

      **Keywords**

      * `vpp::p2p <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Special-Data-Transfer-Models>`__
      * `vpp::h2c <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Special-Data-Transfer-Models>`__
      * `file_buf <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Special-Data-Transfer-Models>`__
      * `sync_output <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/VPP_ACC-Class-API>`__
      * `custom_sync_outputs <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/VPP_ACC-Class-API>`__
      * `sync_output_to_file <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/VPP_ACC-Class-API>`__

  * - `gmio_transfers_sc <gmio_transfers_sc>`_
    - This is gmio transfers example which explains different ways of data movements between host and device for compute unit processing.
    - **Key Concepts**

      * `System Compiler <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Using-Vitis-System-Compilation-Mode>`__
      **Keywords**

      * `DATA_COPY <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Guidance-Macros>`__
      * `ZERO_COPY <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Guidance-Macros>`__
      * `ACCESS_PATTERN <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Guidance-Macros>`__
      * `SEQUENTIAL <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Quick-Start-Example>`__
      * `RANDOM <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Guidance-Macros>`__

  * - `mult_acc_compose_sc <mult_acc_compose_sc>`_
    - In this example we have multiple accelerators in one xclbin, and compose them into a pipeline, with or without CPU processing in-between the PEs.
    - **Key Concepts**

      * Multiple Accelerators

      **Keywords**

      * sync_outputs
      * `custom_sync_outputs <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/VPP_ACC-Class-API>`__

  * - `mult_card_sc <mult_card_sc>`_
    - This is simple example showcasing the P2P and H2C mode of application code controlling a multi-card accelerator design in system compiler environment.
    - **Key Concepts**

      * Multiple Cards

      * `P2P <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/p2p>`__
      * `H2C <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Special-Data-Transfer-Models>`__
      * Syncronization

      **Keywords**

      * `vpp::p2p <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Special-Data-Transfer-Models>`__
      * `vpp::h2c <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Special-Data-Transfer-Models>`__
      * vpp::squeue
      * `add_card <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/CU-Cluster-and-Multi-Card-Support>`__

  * - `quick_start_sc <quick_start_sc>`_
    - This is quick start example based on a video filtering usecase to describe system compiler usage in Vitis Environment.
    - **Key Concepts**

      * `System Compiler <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Using-Vitis-System-Compilation-Mode>`__
      **Keywords**

      * `create_bufpool <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Quick-Start-Example>`__
      * `alloc_buff <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Quick-Start-Example>`__
      * `compute <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/The-compute-API>`__
      * `send_while <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Interface-Data-Types>`__
      * `receive_all_in_order <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/VPP_ACC-Class-API>`__
      * `get_buf <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/VPP_ACC-Class-API>`__
      * `DATA_COPY <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Guidance-Macros>`__
      * `SYS_PORT <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Guidance-Macros>`__
      * `SYS_PORT_PFM <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Guidance-Macros>`__
      * `ACCESS_PATTERN <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Guidance-Macros>`__

  * - `streaming_sc <streaming_sc>`_
    - This is simple example showcasing Free-running PEs with AXI-streams in system compiler environment.
    - **Key Concepts**

      * `Free Running Kernel <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Free-Running-Kernel>`__
      **Keywords**

      * `vpp::stream <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Stream-connections-using-vpp-stream>`__
      * `hls::axis <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/AXI4-Stream-Interfaces>`__
      * `hls::stream <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/HLS-Stream-Library>`__


