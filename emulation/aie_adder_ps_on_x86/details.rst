AIE Adder Embedded SW-Emulation using x86 PS
============================================

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
