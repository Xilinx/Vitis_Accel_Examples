IOPS Test
=========

This is simple test design to measure Input/Output Operations per second. In this design, a simple kernel is enqueued many times and measuring overall IOPS.

**KEY CONCEPTS:** Input/Output Operations per second

**KEYWORDS:** CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE

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
   src/krnl_cntr.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./iops_test <krnl_cntr XCLBIN>

