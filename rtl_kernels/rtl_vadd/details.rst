Vadd Simple (RTL Kernel)
========================

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
