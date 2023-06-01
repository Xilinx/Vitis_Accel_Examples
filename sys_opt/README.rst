System Optimization Examples
==================================
System Optimization examples focussed towards hardware optimization using HLS C/C++ kernels for Xilinx devices.

**Examples Table :**

.. list-table:: 
  :header-rows: 1

  * - **Example**
    - **Description**
    - **Key Concepts/Keywords**
  * - `advanced_config <advanced_config>`_
    - This is simple example to describe how user can provide various advanced options in Vitis flow.
    - 
      **Key Concepts**

      * `Using custom Tcl <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/hls-Options>`__
      * `Setting Kernel Clock Frequency <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Managing-Clock-Frequencies>`__
      * `Clock Uncertainity <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/hls-Options>`__
      **Keywords**

      * `config <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/v-General-Options>`__
      * set_clock_uncertainity
      * `clock <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/clock-Options>`__
      * `freqHz <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/clock-Options>`__
      * `hls.pre_tcl <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/hls-Options>`__

  * - `kernel_swap <kernel_swap>`_
    - This example shows how host can swap the kernels and share same buffer between two kernels which are exist in separate binary containers. Dynamic platforms does not persist the buffer data so host has to migrate data from device to host memory before swapping the next kernel. After kernel swap, host has to migrate the buffer back to device.
    - 
      **Key Concepts**

      * Handling Buffer sharing across multiple binaries

      * Multiple Kernel Binaries

      **Keywords**

      * `enqueueMigrateMemObjects <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Buffer-Creation-and-Data-Transfer>`__
      * `CL_MIGRATE_MEM_OBJECT_HOST <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Sub-Buffers>`__

  * - `multiple_devices <multiple_devices>`_
    - This example show how to take advantage of multiple FPGAs on a system. It will show how to initialized an OpenCL context, allocate memory on the two devices and execute a kernel on each FPGA.
    - 
      **Key Concepts**

      * `OpenCL Host APIs <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/OpenCL-Programming>`__
      * Multi-FPGA Execution

      * `Event Handling <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Overlapping-Data-Transfers-with-Kernel-Computation>`__
      **Keywords**

      * xcl::get_xil_devices

  * - `multiple_process <multiple_process>`_
    - This example will demonstrate how to run multiple processes to utilize multiple kernels simultaneously on an FPGA device. Multiple processes can share access to the same device provided each process uses the same xclbin. Processes share access to all device resources but there is no support for exclusive access to resources by any process.
    - 
      **Key Concepts**

      * `Concurrent execution <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Task-Parallelism-Using-Different-Kernels>`__
      * Multiple HLS kernels

      * Multiple Process Support

      **Keywords**

      * PID
      * fork
      * XCL_MULTIPROCESS_MODE
      * multiprocess

  * - `plram_config <plram_config>`_
    - This example showcases how to configure various PLRAM parameters like Memory Size, Memory Type(BRAM/URAM) etc.
    - 
      **Key Concepts**

      * Vitis Memory Hierarchy

      * `PLRAMs <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Assigning-AXI-Interfaces-to-PLRAM>`__
      **Keywords**

      * `PLRAM <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Assigning-AXI-Interfaces-to-PLRAM>`__
      * BRAM
      * URAM

  * - `slr_assign <slr_assign>`_
    - This is simple example to describe SLR assignment information for a platform design. This example highlights how to provide extra input to assign the logic of the kernel into a nominated SLR. In this example we are assigning first kernel(Vector Multiplication) to SLR0 and assigning the second kernel(Vector Addition) to SLR1
    - 
      **Key Concepts**

      * `SLR Assignments <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Assigning-Kernels-to-SLRs>`__
      **Keywords**

      * `slr <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/connectivity-Options>`__


