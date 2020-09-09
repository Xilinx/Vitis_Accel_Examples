Loop Reordering (OpenCL Kernel)
===============================

This is a simple example of matrix multiplication (Row x Col) to demonstrate how to achieve better pipeline II factor by loop reordering.

**KEY CONCEPTS:** Kernel Optimization, Loop reorder to improve II

**KEYWORDS:** xcl_pipeline_loop, xcl_array_partition

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/mmult.cl
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./cl_loop_reorder <mmult XCLBIN>

