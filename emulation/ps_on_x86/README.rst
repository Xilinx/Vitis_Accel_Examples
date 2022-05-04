Embedded SW-Emulation using x86 PS (HLS C/C++ Kernel)
=====================================================

This example demonstrates how a user can run the PS App as an x86 process instead of ARM process.

**KEY CONCEPTS:** `Compiling and Linking for x86 <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Compiling-and-Linking-for-x86>`__, `Compiling and Linking for Arm <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Compiling-and-Linking-for-Arm>`__

.. raw:: html

 <details>

.. raw:: html

 <summary> 

 <b>EXCLUDED PLATFORMS:</b>

.. raw:: html

 </summary>
|
..

 - All Xilinx DMA Platforms

.. raw:: html

 </details>

.. raw:: html

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/vadd.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./ps_on_x86 <vadd XCLBIN>

DETAILS
-------

This example demonstrates how SW Emulation for Embedded platforms can run the PS App as an x86 process instead of ARM process. SW Emulation is an abstract model and does not use any of the petalinux drivers like Zynq OpenCL (ZOCL), interrupt controller or Device Tree Binary (DTB). Hence, the overhead of creating sd_card.img, booting petalinux on full QEMU machine is too heavy weight and should be avoided for SW Emulation.

There are no changes required in the V++ compile and link steps. For the rest of the stages, following are the differences in the Makefile commands for running the PS App as an x86 process instead of ARM process in case of a PL Kernel-

.. list-table:: 
   :header-rows: 1

   * - Stage
     - ARM Process
     - x86 Process
   * - Host compilation​
     - **aarch64-linux-gnu-g++** -o hello_world host.cpp -lOpenCL
       **--sysroot=$(EDGE_COMMON_SW)/sysroots/cortexa72-cortexa53-xilinx-linux**
       **​​-I$(EDGE_COMMON_SW)/sysroots/cortexa72-cortexa53-xilinx-linux/usr/include/xrt**
       **-L$(EDGE_COMMON_SW)/sysroots/cortexa72-cortexa53-xilinx-linux/usr/lib**
     - **g++** -o hello_world host.cpp
       -lOpenCL
       **-I$(XILINX_XRT)/include**
       **-L$(XILINX_XRT)/lib​​**
   * - Package​
     - $(VPP) -p  -t sw_emu $(LINK_XCLBIN) $(LIBADF)
       --platform $(PLATFORM_PATH)
       --package.out_dir ./package.hw_emu
       **--package.rootfs $(EDGE_COMMON_SW)/rootfs.ext4**
       **--package.kernel_image $(EDGE_COMMON_SW)/Image**
       **--package.sd_file xrt.ini**
       **--package.sd_file ./run_app.sh**
       -o vadd.xclbin
       --package.sd_file ./hello_world
     - $(VPP) -p  -t sw_emu $(LINK_XCLBIN) $(LIBADF)
       --platform $(PLATFORM_PATH) ​**--package.ps_on_x86** --package.out_dir ./package.sw_emu -o $(XCLBIN)​
   * - Simulation Launch​​
     - **./launch_sw_emu.sh -run-app $(RUN_APP_SCRIPT) | tee run_app.log;​​**
     - **XCL_EMULATION_MODE=$(TARGET) ./$(PS_APP) $(PS_APP_ARGS)​​**

For sw_emu, XRT needs to be installed on the host whereas for hw_emu and hw the user is expected to provide the EDGE_COMMON_SW path.

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.