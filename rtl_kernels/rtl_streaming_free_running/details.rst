Stream Free Running (RTL Kernel)
================================

This example demonstrates the use of RTL ``free running`` kernel. Free
running kernel starts as soon as it is out of reset and never stops.
Free running kernels can only have ``stream interfaces`` for
communication with the host. Kernel simply increments the input by 1 and
returns the result.

RTL kernels can be integrated to Vitis using ``RTL Kernel Wizard``.
These kernels have the same software interface model as OpenCL and C/C++
kernels. That is, they are seen by the host application as functions
with a void return value, scalar arguments and pointer arguments. For
free running kernels, Option ``ap_ctrl_none`` has to be chosen while
setting up the kernel in the wizard.

The RTL Kernel Wizard automates some of the steps that need to be taken
to ensure that the RTL IP is packaged into a kernel that can be
integrated into a system in Vitis environment. A ``kernel.xml`` file is
generated to match the software function prototype and behavior
specified in the wizard.
