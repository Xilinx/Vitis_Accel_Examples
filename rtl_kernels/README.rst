RTL Kernel Examples
==================================
RTL Kernel Based Examples

**Examples Table :**

.. list-table:: 
  :header-rows: 1

  * - **Example**
    - **Description**
    - **Key Concepts/Keywords**
  * - `rtl_adder_streams <rtl_adder_streams>`_
    - This example shows an adder with streams using 3 RTL kernels.
    - 
      **Key Concepts**

      * `RTL Kernel <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Generate-RTL-Kernel>`__
      * `Multiple RTL Kernels <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Generate-RTL-Kernel>`__

  * - `rtl_streaming_free_running_k2k <rtl_streaming_free_running_k2k>`_
    - This is simple example which demonstrate how to use and configure a free running RTL kernel.
    - 
      **Key Concepts**

      * `Read/Write Stream <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Writing-the-Software-Application>`__
      * `RTL Kernel <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Generate-RTL-Kernel>`__
      * `Free Running Kernel <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Free-Running-Kernel>`__
      **Keywords**

      * `ap_axiu <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Streaming-Data-Transfers>`__
      * `hls::stream <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/HLS-Stream-Library>`__

  * - `rtl_streaming_k2k_mm <rtl_streaming_k2k_mm>`_
    - This example uses two simple increment RTL kernels connected to each other via stream interface and connected to host via memory mapped C++ data mover kernels
    - 
      **Key Concepts**

      * `Read/Write Stream <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Writing-the-Software-Application>`__
      * `RTL Kernel <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Generate-RTL-Kernel>`__
      **Keywords**

      * `ap_axiu <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Streaming-Data-Transfers>`__
      * `hls::stream <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/HLS-Stream-Library>`__

  * - `rtl_user_managed <rtl_user_managed>`_
    - Simple example of user managed RTL Kernel.
    - 
      **Key Concepts**

      * `User-Managed RTL Kernel <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Creating-User-Managed-RTL-Kernels>`__
      **Keywords**

      * `package_xo <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/package_xo-Command>`__
      * `ctrl_protocol <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/package_xo-Command>`__
      * `user_managed <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Creating-User-Managed-RTL-Kernels>`__
      * `xrt::ip <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Working-with-User-Managed-Kernels>`__
      * `xrt::xclbin <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Specifying-the-Device-ID-and-Loading-the-XCLBIN>`__
      * xrt::kernel::get_kernels
      * xrt::kernel::get_cus
      * xrt::kernel::get_args
      * xrt::arg::get_offset
      * `xrt::ip::write_register <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Working-with-User-Managed-Kernels>`__
      * xrt::ip::read_register

  * - `rtl_vadd <rtl_vadd>`_
    - This is a simple example of vector addition using RTL kernel and supports all flows including sw_emu using C-Model.
    - 
      **Key Concepts**

      * `RTL Kernel <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Generate-RTL-Kernel>`__
      * `RTL C-Model <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Software-Model-and-Host-Code-Example>`__
      **Keywords**

      * `kernel_files <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Packaging-the-RTL-Code-as-a-Vitis-XO>`__

  * - `rtl_vadd_2clks <rtl_vadd_2clks>`_
    - This example shows vector addition with 2 kernel clocks using RTL Kernel.
    - 
      **Key Concepts**

      * `RTL Kernel <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Generate-RTL-Kernel>`__
      * `Multiple Kernel Clocks <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/hls-Options>`__
      **Keywords**

      * `clock <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/clock-Options>`__
      * `freqHz <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/clock-Options>`__
      * `ap_clk <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Control-Clock-and-Reset-in-AXI4-Lite-Interfaces>`__
      * ap_clk2

  * - `rtl_vadd_2kernels <rtl_vadd_2kernels>`_
    - This example has two RTL Kernels. Both Kernel_0 and Kernel_1 perform vector addition. The Kernel_1 reads the output from Kernel_0 as one of two inputs.
    - 
      **Key Concepts**

      * `Multiple RTL Kernels <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Generate-RTL-Kernel>`__

  * - `rtl_vadd_hw_debug <rtl_vadd_hw_debug>`_
    - This is an example that showcases the Hardware Debug of Vector Addition RTL Kernel in Hardware.
    - 
      **Key Concepts**

      * `RTL Kernel Debug <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Adding-Debug-IP-to-RTL-Kernels>`__

  * - `rtl_vadd_mixed_c_vadd <rtl_vadd_mixed_c_vadd>`_
    - This example has one RTL kernel and one C kernel. Both RTL kernel and C kernel perform vector addition. The C kernel reads the output from RTL kernel as one of two inputs.
    - 
      **Key Concepts**

      * Mixed Kernels



