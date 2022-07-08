Emulation Examples
==================================
This section contains Emulation Examples.

**Examples Table :**

.. list-table:: 
  :header-rows: 1

  * - **Example**
    - **Description**
    - **Key Concepts/Keywords**
  * - `aie_adder_ps_on_x86 <aie_adder_ps_on_x86>`_
    - This example demonstrates how a user can run the PS App as an x86 process instead of ARM process for an AIE design.
    - **Key Concepts**

      * `Compiling and Linking for x86 <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Compiling-and-Linking-for-x86>`__
      * `Compiling and Linking for Arm <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Compiling-and-Linking-for-Arm>`__
      * `AI Engine <https://docs.xilinx.com/r/en-US/ug1076-ai-engine-environment/AI-Engine/Programmable-Logic-Integration>`__
      * `Adaptive Data Flow <https://docs.xilinx.com/r/en-US/ug1076-ai-engine-environment/Adaptive-Data-Flow-Graph-Specification-Reference>`__
      * `Graph <https://docs.xilinx.com/r/en-US/ug1076-ai-engine-environment/graph>`__
      **Keywords**

      * `aiecompiler <https://docs.xilinx.com/r/en-US/ug1076-ai-engine-environment/Compiling-an-AI-Engine-Graph-Application>`__
      * `aiesimulator <https://docs.xilinx.com/r/en-US/ug1076-ai-engine-environment/Simulating-an-AI-Engine-Graph-Application>`__
      * `stream_connect <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Specifying-Streaming-Connections-between-Compute-Units>`__

  * - `aie_baremetal <aie_baremetal>`_
    - This is simple AIE example to demonstrate Vitis Bare-Metal Emulation flow on versal platform.
    - **Key Concepts**

      * `Building a Bare-Metal System <https://docs.xilinx.com/r/en-US/ug1076-ai-engine-environment/Building-a-Bare-Metal-System>`__

  * - `deadlock_streaming_k2k_mm <deadlock_streaming_k2k_mm>`_
    - This is a simple kernel to kernel streaming Vector Add and Vector Multiply C Kernel design to demonstrate system level deadlock detection feature in HW Emulation.
    - **Key Concepts**

      * System Level Deadlock Detection

      **Keywords**

      * systemDeadlockDetection=enable

  * - `external_io_cpp <external_io_cpp>`_
    - This example will demonstrate emulation of Xilinx Device's IO with a C++ based external process .
    - **Key Concepts**

      * `Traffic Generator <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Using-I/O-Traffic-Generators>`__
      * `Traffic generation using C++ <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Writing-Traffic-Generators-in-C>`__
      **Keywords**

      * `sim_ipc_axis_master <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Adding-Traffic-Generators-to-Your-Design>`__
      * `sim_ipc_axis_slave <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Adding-Traffic-Generators-to-Your-Design>`__
      * `gt_master <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Using-I/O-Traffic-Generators>`__
      * `gt_slave <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Using-I/O-Traffic-Generators>`__

  * - `external_io_py <external_io_py>`_
    - This example will demonstrate emulation of Xilinx Device's IO with a Python based external process.
    - **Key Concepts**

      * `Traffic Generator <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Using-I/O-Traffic-Generators>`__
      * `Traffic generation using Python <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Writing-Traffic-Generators-in-Python>`__
      **Keywords**

      * `sim_ipc_axis_master <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Adding-Traffic-Generators-to-Your-Design>`__
      * `sim_ipc_axis_slave <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Adding-Traffic-Generators-to-Your-Design>`__
      * `gt_master <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Using-I/O-Traffic-Generators>`__
      * `gt_slave <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Using-I/O-Traffic-Generators>`__

  * - `platform_validate <platform_validate>`_
    - This is simple example of vector addition to demonstrate Vitis Bare-Metal Emulation test on versal platform.
    - **Key Concepts**

      * `Building a Bare-Metal System <https://docs.xilinx.com/r/en-US/ug1076-ai-engine-environment/Building-a-Bare-Metal-System>`__

  * - `ps_on_x86 <ps_on_x86>`_
    - This example demonstrates how a user can run the PS App as an x86 process instead of ARM process.
    - **Key Concepts**

      * `Compiling and Linking for x86 <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Compiling-and-Linking-for-x86>`__
      * `Compiling and Linking for Arm <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Compiling-and-Linking-for-Arm>`__

  * - `stream_func_mode <stream_func_mode>`_
    - This example demonstrates how a user can run functional model of HLS kernel in HW Emulation.
    - **Key Concepts**

      * `Emulation Functional mode <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Working-with-Functional-Model-of-the-HLS-Kernel>`__
      **Keywords**

      * `emulationMode=func <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Working-with-Functional-Model-of-the-HLS-Kernel>`__

  * - `systemc_vadd <systemc_vadd>`_
    - This is simple example of vector addition to describe usage of custom SystemC in Vitis Environment.
    - **Key Concepts**

      * `SystemC <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Working-with-SystemC-Models>`__
      **Keywords**

      * `create_sc_xo <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Working-with-SystemC-Models>`__
      * `xtlm_ap_ctrl_hs <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Coding-a-SystemC-Model>`__


