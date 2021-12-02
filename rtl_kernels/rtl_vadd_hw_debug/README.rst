Hardware Debug (RTL Kernel)
===========================

This is an example that showcases the Hardware Debug of Vector Addition RTL Kernel in Hardware.

 
**For more information on the steps involved in debugging Vitis RTL Kernel Designs check the links below**
 
[Debug Core setup in the RTL kernel design and host design modifications to allow setup of debug triggers prior to running the kernel](https://github.com/Xilinx/Vitis_Accel_Examples/wiki/rtl_kernel_debug)

**KEY CONCEPTS:** `RTL Kernel Debug <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Adding-Debug-IP-to-RTL-Kernels>`__

**EXCLUDED PLATFORMS:** 

 - All Versal Platforms, i.e vck190 etc
 - All NoDMA Platforms, i.e u50 nodma etc

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/hdl/krnl_vadd_rtl.v
   src/hdl/krnl_vadd_rtl_adder.sv
   src/hdl/krnl_vadd_rtl_axi_read_master.sv
   src/hdl/krnl_vadd_rtl_axi_write_master.sv
   src/hdl/krnl_vadd_rtl_control_s_axi.v
   src/hdl/krnl_vadd_rtl_counter.sv
   src/hdl/krnl_vadd_rtl_int.sv
   src/host.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./rtl_vadd_hw_debug <vadd XCLBIN>

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.