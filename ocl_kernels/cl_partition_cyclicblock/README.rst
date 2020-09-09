Array Block and Cyclic Partitioning (OpenCL Kernel)
===================================================

This example shows how to use array block and cyclic partitioning to improve performance of a kernel

**KEY CONCEPTS:** Kernel Optimization, Array Partitioning, Block Partition, Cyclic Partition

**KEYWORDS:** xcl_array_partition, cyclic, block

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   zc702

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/matmul.cl
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./cl_partition_cyclicblock <matmul XCLBIN>

