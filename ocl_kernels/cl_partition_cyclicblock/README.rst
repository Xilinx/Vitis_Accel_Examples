Array Block and Cyclic Partitioning (OpenCL Kernel)
===================================================

This example shows how to use array block and cyclic partitioning to improve performance of a kernel

**KEY CONCEPTS:** `Kernel Optimization <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Kernel-Optimization>`__, Array Partitioning, `Block Partition <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-array_partition>`__, `Cyclic Partition <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-array_partition>`__

**KEYWORDS:** `xcl_array_partition <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/xcl_array_partition>`__, `cyclic <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-array_partition>`__, `block <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/pragma-HLS-array_partition>`__

**EXCLUDED PLATFORMS:** 

 - Embedded ZC702
 - All NoDMA Platforms, i.e u50 nodma etc

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

DETAILS
-------

This example demonstrates how to use array ``block`` and ``cyclic``
partitioning to improve the performance of the kernel. Matrix
multiplication is performed in this example which would require repeated
access to rows of former matrix and columns of latter. Array can be
partitioned across different dimensions to reduce the latency of these
transfers.

``xcl_array_partition`` attribute is used to partition an array into
multiple smaller arrays with more number of ports for read and write
operations. This results in improved throughput of the design.

Arrays can be partitioned in three ways, ``cyclic`` where elements are
put into smaller arrays one by one in the interleaved manner untill the
whole array is partitioned, ``block`` where elements are put into
smaller arrays from continuous blocks of original array and ``complete``
where array is decomposed into individual elements each having own
read/write ports.

.. code:: cpp

    int A[MAX_DIM * MAX_DIM] __attribute__((xcl_array_partition(cyclic, MAX_DIM, 1)));
    int B[MAX_DIM * MAX_DIM] __attribute__((xcl_array_partition(block, MAX_DIM, 1)));

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.