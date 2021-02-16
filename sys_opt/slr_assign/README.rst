SLR Assign (CL)
===============

This is simple example to describe SLR assignment information for a platform design. This example highlights how to provide extra input to assign the logic of the kernel into a nominated SLR. In this example we are assigning first kernel(Vector Multiplication) to SLR0 and assigning the second kernel(Vector Addition) to SLR1

**KEY CONCEPTS:** SLR Assignments

**KEYWORDS:** slr

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   _u25_
   u30
   zc
   vck
   samsung
   _u2_
   nodma

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/kernel.cl
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./slr_assign <vmult_vadd XCLBIN>

DETAILS
-------

This example demonstrates how kernels can be assigned to different
SLRs(Super Logic Region) during the kernel linking stage.

Actual kernel resource utilization values can be used to help distribute
CUs across SLRs to reduce congestion in any one SLR. Xilinx recommends
connecting to a DDR memory resource in the same SLR as the CU. This
reduces competition for the limited ``SLR-crossing`` connection
resources. In addition, connections between SLRs use super long line
(SLL) routing resources, which incurs a greater delay than a standard
intra-SLR routing.

It might be necessary to cross an SLR region to connect to a DDR
resource in a different SLR. However, if both the and the directives are
explicitly defined, the tools ``--sp``, ``--slr`` automatically add
additional crossing logic to minimize the effect of the SLL delay, and
facilitates better timing closure.

``V++`` requires ``--slr`` option during the linking stage for explicit
slr assignment of kernels.

::

   --slr vmult_1:SLR0 --slr vadd_1:SLR1

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.