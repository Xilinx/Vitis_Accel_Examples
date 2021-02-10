Mixed Kernels (RTL & C)
=======================

This example has one RTL kernel and one C kernel. Both RTL kernel and C kernel perform vector addition. The C kernel reads the output from RTL kernel as one of two inputs.

**KEY CONCEPTS:** Mixed Kernels

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   nodma

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
   src/krnl_vadd.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./rtl_vadd_mixed_c_vadd <vadd XCLBIN>

DETAILS
-------

This example demonstrates how we can have RTL as well as C kernels in a
single application. Both RTL kernel, ``rtl_vadd`` and C kernel,
``cl_vadd`` perform the vector addition. C kernel reads the result of
addition by the RTL kernel as one of its inputs and adds another vector.

RTL kernels can be integrated to Vitis using ``RTL Kernel Wizard``.
These kernels have the same software interface model as OpenCL and C/C++
kernels. That is, they are seen by the host application as functions
with a void return value, scalar arguments, and pointer arguments.

The RTL Kernel Wizard automates some of the steps that need to be taken
to ensure that the RTL IP is packaged into a kernel that can be
integrated into a system in Vitis environment.

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.