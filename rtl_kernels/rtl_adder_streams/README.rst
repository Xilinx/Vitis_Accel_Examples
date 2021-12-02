Adder With Streams (RTL Kernel)
===============================

This example shows an adder with streams using 3 RTL kernels.

**KEY CONCEPTS:** `RTL Kernel <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/RTL-Kernels>`__, `Multiple RTL Kernels <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/RTL-Kernels>`__

**EXCLUDED PLATFORMS:** 

 - All NoDMA Platforms, i.e u50 nodma etc

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/ip/ip_1/component.xml
   src/ip/ip_1/src/krnl_input_stage_rtl.v
   src/ip/ip_1/src/krnl_input_stage_rtl_axi_read_master.sv
   src/ip/ip_1/src/krnl_input_stage_rtl_control_s_axi.v
   src/ip/ip_1/src/krnl_input_stage_rtl_counter.sv
   src/ip/ip_1/src/krnl_input_stage_rtl_int.sv
   src/ip/ip_1/xgui/krnl_input_stage_rtl_v1_0.tcl
   src/ip/ip_2/component.xml
   src/ip/ip_2/src/krnl_adder_stage_rtl.v
   src/ip/ip_2/src/krnl_adder_stage_rtl_adder.sv
   src/ip/ip_2/src/krnl_adder_stage_rtl_control_s_axi.v
   src/ip/ip_2/src/krnl_adder_stage_rtl_counter.sv
   src/ip/ip_2/src/krnl_adder_stage_rtl_int.sv
   src/ip/ip_2/xgui/krnl_adder_stage_rtl_v1_0.tcl
   src/ip/ip_3/component.xml
   src/ip/ip_3/src/krnl_output_stage_rtl.v
   src/ip/ip_3/src/krnl_output_stage_rtl_axi_write_master.sv
   src/ip/ip_3/src/krnl_output_stage_rtl_control_s_axi.v
   src/ip/ip_3/src/krnl_output_stage_rtl_counter.sv
   src/ip/ip_3/src/krnl_output_stage_rtl_int.sv
   src/ip/ip_3/src/krnl_output_stage_rtl_register_slice.sv
   src/ip/ip_3/xgui/krnl_output_stage_rtl_v1_0.tcl
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./rtl_adder_streams <adder XCLBIN>

DETAILS
-------

This example demonstrates the use of RTL kernels for addition with
stream interfaces. Kernel simply increments the input by 1 and returns
the result.

RTL kernels can be integrated to Vitis using ``RTL Kernel Wizard``.
These kernels have the same software interface model as OpenCL and C/C++
kernels. That is, they are seen by the host application as functions
with a void return value, scalar arguments, and pointer arguments.

The RTL Kernel Wizard automates some of the steps that need to be taken
to ensure that the RTL IP is packaged into a kernel that can be
integrated into a system in Vitis environment.

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.