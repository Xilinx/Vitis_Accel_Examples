Vadd Simple (RTL Kernel)
========================

This is a simple example of vector addition using RTL kernel and supports all flows including sw_emu using C-Model.

**KEY CONCEPTS:** `RTL Kernel <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/RTL-Kernels>`__, `RTL C-Model <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Software-Model-and-Host-Code-Example>`__

**KEYWORDS:** `kernel_files <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Packaging-the-RTL-Code-as-a-Vitis-XO>`__

**EXCLUDED PLATFORMS:** 

 - All NoDMA Platforms, i.e u50 nodma etc

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/krnl_vadd/gen_xo.tcl
   src/krnl_vadd/hdl/krnl_vadd_rtl.v
   src/krnl_vadd/hdl/krnl_vadd_rtl_adder.sv
   src/krnl_vadd/hdl/krnl_vadd_rtl_axi_read_master.sv
   src/krnl_vadd/hdl/krnl_vadd_rtl_axi_write_master.sv
   src/krnl_vadd/hdl/krnl_vadd_rtl_control_s_axi.v
   src/krnl_vadd/hdl/krnl_vadd_rtl_counter.sv
   src/krnl_vadd/hdl/krnl_vadd_rtl_int.sv
   src/krnl_vadd/package_kernel.tcl
   src/krnl_vadd/vadd_cmodel.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./rtl_vadd <vadd XCLBIN>

DETAILS
-------

This example demonstrates simple vector addition with RTL kernel. Two
vectors are transferred from host to kernel, added and the result is
written back to host and verified.

RTL kernels can be integrated to Vitis using ``RTL Kernel Wizard``.
These kernels have the same software interface model as OpenCL and C/C++
kernels. That is, they are seen by the host application as functions
with a void return value, scalar arguments, and pointer arguments.

The RTL Kernel Wizard automates some of the steps that need to be taken
to ensure that the RTL IP is packaged into a kernel that can be
integrated into a system in Vitis environment.

The example also supports sw_emu by running the vadd.cpp code provided in the ``package_xo`` command as follows:

::

   package_xo -xo_path ${xoname} -kernel_name krnl_vadd_rtl -ip_directory ./packaged_kernel_${suffix} -kernel_files src/krnl_vadd/vadd_CModel.cpp

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.