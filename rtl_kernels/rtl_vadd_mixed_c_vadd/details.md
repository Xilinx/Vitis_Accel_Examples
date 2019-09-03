Mixed Kernels (RTL & C)
========================

This example demonstrates how we can have RTL as well as C kernels in a single application. Both RTL kernel, `rtl_vadd` and C kernel, `cl_vadd` perform the vector addition. C kernel reads the result of addition by the RTL kernel as one of its inputs and adds another vector.

RTL kernels can be integrated to Vitis using `RTL Kernel Wizard`. These kernels have the same software interface model as OpenCL and C/C++ kernels. That is, they are seen by the host application as functions with a void return value, scalar arguments, and pointer arguments.

The RTL Kernel Wizard automates some of the steps that need to be taken to ensure that the RTL IP is packaged into a kernel that can be integrated into a system in Vitis environment. A `kernel.xml` file is generated to match the software function prototype and behavior
specified in the wizard.
