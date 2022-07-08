AIE Adder Embedded SW-Emulation using x86 PS 
=============================================

This example demonstrates how a user can run the PS App as an x86 process instead of ARM process for an AIE design.

**KEY CONCEPTS:** `Compiling and Linking for x86 <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Compiling-and-Linking-for-x86>`__, `Compiling and Linking for Arm <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Compiling-and-Linking-for-Arm>`__, `AI Engine <https://docs.xilinx.com/r/en-US/ug1076-ai-engine-environment/AI-Engine/Programmable-Logic-Integration>`__, `Adaptive Data Flow <https://docs.xilinx.com/r/en-US/ug1076-ai-engine-environment/Adaptive-Data-Flow-Graph-Specification-Reference>`__, `Graph <https://docs.xilinx.com/r/en-US/ug1076-ai-engine-environment/graph>`__

**KEYWORDS:** `aiecompiler <https://docs.xilinx.com/r/en-US/ug1076-ai-engine-environment/Compiling-an-AI-Engine-Graph-Application>`__, `aiesimulator <https://docs.xilinx.com/r/en-US/ug1076-ai-engine-environment/Simulating-an-AI-Engine-Graph-Application>`__, `stream_connect <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Specifying-Streaming-Connections-between-Compute-Units>`__

.. raw:: html

 <details>

.. raw:: html

 <summary> 

 <b>EXCLUDED PLATFORMS:</b>

.. raw:: html

 </summary>
|
..

 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - AWS VU9P F1
 - Samsung SmartSSD Computation Storage Drive
 - Samsung U.2 SmartSSD
 - Versal VCK5000
 - All Xilinx DMA Platforms

.. raw:: html

 </details>

.. raw:: html

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   data/golden.txt
   data/input0.txt
   data/input1.txt
   src/aie/adder.cc
   src/aie/graph.cpp
   src/aie/graph.h
   src/aie/kernel.h
   src/pl_kernels/mm2s.cpp
   src/pl_kernels/s2mm.cpp
   src/sw/host.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./aie_adder_ps_on_x86 <krnl_adder XCLBIN>

DETAILS
-------

This example demonstrates how SW Emulation for Embedded platforms can run the PS App as an x86 process instead of ARM process for an AIE Design. SW Emulation is an abstract model and does not use any of the petalinux drivers like Zynq OpenCL (ZOCL), interrupt controller or Device Tree Binary (DTB). Hence, the overhead of creating sd_card.img, booting petalinux on full QEMU machine is too heavy weight and should be avoided for SW Emulation.

There are no changes required in the V++ compile and link steps. For the rest of the stages, following are the differences in the Makefile commands for running the PS App as an x86 process instead of ARM process in case of an AIE Kernel-

.. list-table:: 
   :header-rows: 1

   * - Stage
     - ARM Process
     - x86 Process
   * - Host compilation​
     - GCC_LIB := -lxrt_coreutil **-ladf_api_xrt**
       **-L${XILINX_VITIS}/aietools/lib/aarch64.o -L$(SYSROOT)/usr/lib --sysroot=$(SYSROOT)​**
       GCC_FLAGS := -Wall -c -std=c++14 -Wno-int-to-pointer-cast **--sysroot=$(SYSROOT)**
       GCC_INCLUDES := -I./ -I${XILINX_VITIS}/aietools/include 
       **-I$(SYSROOT)/usr/include/xrt -I$(SYSROOT)/usr/include**
     - GCC_LIB := -lxrt_coreutil  **-ladf_api_xrt_swemu -L${XILINX_VITIS}/aietools/lib/lnx64.o -L${XILINX_XRT}/lib​**
       GCC_FLAGS := -Wall -c -std=c++14 -Wno-int-to-pointer-cast **-I${XILINX_XRT}/include​**
       GCC_INCLUDES := -I./ -I${XILINX_VITIS}/aietools/include​
   * - Package​
     - v++ $(VPP_PFLAGS) -p -t $(TARGET) \​
       --package.defer_aie_run \​
       --platform $(PLATFORM) \​
       --package.out_dir $(PACKAGE_OUT) \​
       $(XCLBIN) $(GRAPH_O) -o krnl_adder.xclbin​ \
       **--package.rootfs $(EDGE_COMMON_SW)/rootfs.ext4** \​
       **--package.image_format=ext4** \​
       **--package.boot_mode=sd** \​
       **--package.kernel_image $(SD_IMAGE_FILE)** \ ​
       **--package.sd_file $(RUN_APP_SCRIPT)** \​
       **--package.sd_file aie_adder_ps_on_x86​**
     - v++ $(VPP_PFLAGS) -p -t sw_emu \​
       --package.defer_aie_run \​
       --platform $(PLATFORM) \​
       --package.out_dir $(PACKAGE_OUT) \​
       $(XCLBIN) $(GRAPH_O) -o krnl_adder.xclbin​
       **--package.ps_on_x86​**
   * - Simulation Launch​​
     - **./launch_sw_emu.sh -run-app $(RUN_APP_SCRIPT) | tee run_app.log;​​**
     - **XCL_EMULATION_MODE=$(TARGET) ./$(PS_APP) $(PS_APP_ARGS)​​**

For sw_emu, XRT needs to be installed on the host whereas for hw_emu and hw the user is expected to provide the EDGE_COMMON_SW path.

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.