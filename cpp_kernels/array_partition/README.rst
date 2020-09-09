Array Partition (HLS C/C++ Kernel)
==================================

This is a simple example of matrix multiplication (Row x Col) to demonstrate how to achieve better performance by array partitioning, using HLS kernel in Vitis Environment.

**KEY CONCEPTS:** Kernel Optimization, HLS C Kernel, Array Partition

**KEYWORDS:** #pragma HLS ARRAY_PARTITION, complete

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/matmul.cpp
   src/matmul_partition.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./array_partition <matmul XCLBIN>

