Embedded SW-Emulation using x86 PS (HLS C/C++ Kernel)
=====================================================

This example demonstrates how a user can run the PS App as an x86 process instead of ARM process.

**KEY CONCEPTS:** `Compiling and Linking for x86 <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Compiling-and-Linking-for-x86>`__, `Compiling and Linking for Arm <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Compiling-and-Linking-for-Arm>`__

**EXCLUDED PLATFORMS:** 

 - All NoDMA Platforms, i.e u50 nodma etc

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

   ./embedded_ps_x86 <vadd XCLBIN>

DETAILS
-------

This example demonstrates how SW Emulation for Embedded platforms can run the PS App as an x86 process instead of ARM process. SW Emulation is an abstract model and does not use any of the petalinux drivers like ZOCL, interrupt controller or DTB. Hence, the overhead of creating sd_card.img, booting petalinux on full QEMU machine is too heavy weight and should be avoided for SW Emulation.



For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.