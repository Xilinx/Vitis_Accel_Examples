Stream Free Running Kernel (RTL Kernel)
=======================================

This is a simple Vector Increment RTL Kernel design with 1 Stream input and 1 Stream output that demonstrates on how to process an input stream of data for computation in an application.

**KEY CONCEPTS:** Read/Write Stream, Create/Release Stream, RTL Kernel

**KEYWORDS:** cl_stream, CL_STREAM_EOT

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   u30
   u250
   zc
   vck
   xdma
   qep
   aws-vu9p-f1
   samsung
   nodma

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/gen_xo.tcl
   src/host.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./rtl_streaming_free_running <myadder1 XCLBIN>

DETAILS
-------

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

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.