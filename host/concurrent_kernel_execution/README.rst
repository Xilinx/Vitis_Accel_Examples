Concurrent Kernel Execution (C)
===============================

This example will demonstrate how to use multiple and out of order command queues to simultaneously execute multiple kernels on an FPGA.

**KEY CONCEPTS:** Concurrent execution, Out of Order Command Queues, Multiple Command Queues

**KEYWORDS:** CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, setCallback

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/madd.cpp
   src/mmult.cpp
   src/mscale.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./concurrent_kernel_execution <matrix_ops XCLBIN>

