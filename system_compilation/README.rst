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

      * Debug

      * Profiling

      **Keywords**

      * PROFILE_KERNEL
      * PROFILE_PORT
      * PROFILE_OFFLOAD
      * vpp::stream<>

  * - `eth_acc_sc <eth_acc_sc>`_
    - This is simple example showcasing custom AXI-streams to PE interfaces in system compiler environment.
    - **Key Concepts**

      * Asyncronous host interactions

      * Custom AXI-Streams

      **Keywords**

      * compute_async

  * - `file_filter_sc <file_filter_sc>`_
    - This is simple example showcasing the P2P and H2C file transfers in system compiler environment.
    - **Key Concepts**

      * `P2P <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/p2p>`__
      * SSD

      * Syncronization

      **Keywords**

      * vpp::p2p
      * vpp::h2c
      * file_buf
      * sync_output
      * custom_sync_outputs
      * sync_output_to_file

  * - `gmio_transfers_sc <gmio_transfers_sc>`_
    - This is gmio transfers example which explains different ways of data movements between host and device for compute unit processing.
    - **Key Concepts**

      * System Compiler

      **Keywords**

      * DATA_COPY
      * ZERO_COPY
      * ACCESS_PATTERN
      * SEQUENTIAL
      * RANDOM

  * - `mult_acc_compose_sc <mult_acc_compose_sc>`_
    - In this example we have multiple accelerators in one xclbin, and compose them into a pipeline, with or without CPU processing in-between the PEs.
    - **Key Concepts**

      * Multiple Accelerators

      **Keywords**

      * sync_outputs
      * custom_sync_outputs

  * - `mult_card_sc <mult_card_sc>`_
    - This is simple example showcasing the P2P and H2C mode of application code controlling a multi-card accelerator design in system compiler environment.
    - **Key Concepts**

      * Multiple Cards

      * `P2P <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/p2p>`__
      * H2C

      * Syncronization

      **Keywords**

      * vpp::p2p
      * vpp::h2c
      * vpp::squeue
      * add_card

  * - `quick_start_sc <quick_start_sc>`_
    - This is quick start example based on a video filtering usecase to describe system compiler usage in Vitis Environment.
    - **Key Concepts**

      * System Compiler

      **Keywords**

      * create_bufpool
      * alloc_buff
      * compute
      * send_while
      * receive_all_in_order
      * get_buf
      * DATA_COPY
      * SYS_PORT
      * SYS_PORT_PFM
      * ACCESS_PATTERN

  * - `streaming_sc <streaming_sc>`_
    - This is simple example showcasing Free-running PEs with AXI-streams in system compiler environment.
    - **Key Concepts**

      * `Free Running Kernel <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Free-Running-Kernel>`__
      **Keywords**

      * vpp::stream
      * hls::axis
      * `hls::stream <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/HLS-Stream-Library>`__


