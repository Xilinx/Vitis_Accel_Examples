Vadd 2 Kernels (RTL Kernel)
===========================

This example has two RTL Kernels. Both Kernel_0 and Kernel_1 perform vector addition. The Kernel_1 reads the output from Kernel_0 as one of two inputs.

**KEY CONCEPTS:** `Multiple RTL Kernels <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devrtlkernel.html>`__

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   nodma

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/ip_0/hdl/krnl_vadd_rtl_0.v
   src/ip_0/hdl/krnl_vadd_rtl_adder.sv
   src/ip_0/hdl/krnl_vadd_rtl_axi_read_master.sv
   src/ip_0/hdl/krnl_vadd_rtl_axi_write_master.sv
   src/ip_0/hdl/krnl_vadd_rtl_control_s_axi.v
   src/ip_0/hdl/krnl_vadd_rtl_counter.sv
   src/ip_0/hdl/krnl_vadd_rtl_int.sv
   src/ip_1/hdl/krnl_vadd_rtl_1.v
   src/ip_1/hdl/krnl_vadd_rtl_adder.sv
   src/ip_1/hdl/krnl_vadd_rtl_axi_read_master.sv
   src/ip_1/hdl/krnl_vadd_rtl_axi_write_master.sv
   src/ip_1/hdl/krnl_vadd_rtl_control_s_axi.v
   src/ip_1/hdl/krnl_vadd_rtl_counter.sv
   src/ip_1/hdl/krnl_vadd_rtl_int.sv
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./rtl_vadd_2kernels <vadd XCLBIN>

DETAILS
-------

This example demonstrates the use of two RTL Kernels. 3 input vectors
are added using these two kernels. ``Kernel_1`` performs the vector
addition on 1st two vectors and then ``Kernel_2`` adds the result of
former kernel to the 3rd vector.

RTL kernels can be integrated to Vitis using ``RTL Kernel Wizard``.
These kernels have the same software interface model as OpenCL and C/C++
kernels. That is, they are seen by the host application as functions
with a void return value, scalar arguments, and pointer arguments.

The RTL Kernel Wizard automates some of the steps that need to be taken
to ensure that the RTL IP is packaged into a kernel that can be
integrated into a system in Vitis environment.

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.