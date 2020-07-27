Vadd 2 Kernels (RTL Kernel)
===========================

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
