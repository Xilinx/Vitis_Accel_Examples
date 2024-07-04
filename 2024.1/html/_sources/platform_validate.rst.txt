Platform Validate Bare-Metal Emulation Test 
===========================================
This is simple example to demonstrate Vitis Bare-Metal Emulation flow on Versal platform to validate platform and does not any user PL kernels. 

**KEY CONCEPTS:** `Building the Bare-Metal System <https://docs.xilinx.com/r/en-US/ug1076-ai-engine-environment/Building-a-Bare-metal-System>`__

**EXCLUDED PLATFORMS:** 

 - Samsung SmartSSD Computation Storage Drive
 - Samsung U.2 SmartSSD
 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - All NoDMA Platforms, i.e u50 nodma etc
 
DESIGN FILES
------------

Application code is located in the src directory. 

::

   src/main.cpp 
   
DETAILS
-------

The baremetal flow is used for emulation platform level basic testing i.e. to test different applications - DDR testing, LPDDR testing, AIE testing. This example is to demonstrate simple platform validation.  

In this example, by default extensible hw_emu XSA ``$(PLATFORM_REPO_PATHS)/xilinx_vck190_base_202210_1/hw_emu/hw_emu.xsa`` is directly used at v++ link to generate the fixed XSA. Users can have their own extensible XSA and provide it at v++ link. 

Input from the user
--------------------

The user needs to generate the fixed XSA from v++ link using extensible XSA. 

Steps to run the Bare-Metal Emulation Test
------------------------------------------

1. Build the fixed xsa using v++ link step as per extensible XSA:  
   ``make fixed_xsa EXTENSIBLE_XSA=$(PLATFORM_REPO_PATHS)/xilinx_vck190_base_202210_1/hw_emu/hw_emu.xsa``

2. Build the BSP sources and libraries required for compilation of user application. 
   Compile and link the user application to generate main.elf : ``make baremetal_elf``

* Note: Users can modify sw/main.cpp file and incrementally compile it to build the main.elf as per your user application.

3. Generate the package directory using v++ package step as per fixed platform: ``make package``

* Note: The fixed XPFM generated out of fixed XSA can be found inside ``./workspace_fixed/$(APP_NAME)/export/$(APP_NAME)/$(APP_NAME).xpfm``

4. Run the user application: ``./package.hw_emu/launch_hw_emu.sh``

* To run all the steps at once, use ``make run TARGET=hw_emu EXTENSIBLE_XSA=$(PLATFORM_REPO_PATHS)/xilinx_vck190_base_202210_1/hw_emu/hw_emu.xsa``

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
