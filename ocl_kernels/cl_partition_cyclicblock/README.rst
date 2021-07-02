Array Block and Cyclic Partitioning (OpenCL Kernel)
===================================================

This example shows how to use array block and cyclic partitioning to improve performance of a kernel

**KEY CONCEPTS:** `Kernel Optimization <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitis_hls_optimization_techniques.html>`__, Array Partitioning, `Block Partition <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitis_hls_optimization_techniques.html#swq1539734225427>`__, `Cyclic Partition <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitis_hls_optimization_techniques.html#swq1539734225427>`__

**KEYWORDS:** `xcl_array_partition <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/opencl_programming.html#kal1504034361186>`__, `cyclic <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/hls_pragmas.html#gle1504034361378__ad410829>`__, `block <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/hls_pragmas.html#gle1504034361378__ad410829>`__

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   zc702
   nodma

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