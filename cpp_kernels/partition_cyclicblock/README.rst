Array Block and Cyclic Partitioning (C)
=======================================

This example shows how to use array block and cyclic partitioning to improve performance of a kernel

**KEY CONCEPTS:** `Kernel Optimization <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitis_hls_optimization_techniques.html>`__, Array Partitioning, `Block Partition <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitis_hls_optimization_techniques.html#swq1539734225427>`__, `Cyclic Partition <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitis_hls_optimization_techniques.html#swq1539734225427>`__

**KEYWORDS:** `#pragma HLS ARRAY_PARTITION <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/hls_pragmas.html#gle1504034361378>`__, `cyclic <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/hls_pragmas.html#gle1504034361378__ad410829>`__, `block <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/hls_pragmas.html#gle1504034361378__ad410829>`__, `factor <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/hls_pragmas.html#gle1504034361378__ad410829>`__, `dim <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/hls_pragmas.html#gle1504034361378__ad410829>`__

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
   src/matmul.cpp
   src/matmul_partition.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./partition_cyclicblock <matmul XCLBIN>

DETAILS
-------

This example demonstrates how to use array ``block`` and ``cyclic``
partitioning to improve the performance of the kernel. Matrix
multiplication is performed in this example which would require repeated
access to rows of former matrix and columns of latter. Array can be
partitioned across different dimensions to reduce the latency of these
transfers.

``#pragma HLS ARRAY_PARTITION`` is used to partition an array into
multiple smaller arrays with more number of ports for read and write
operations. This results in improved throughput of the design.

Arrays can be partitioned in three ways, ``cyclic`` where elements are
put into smaller arrays one by one in the interleaved manner untill the
whole array is partitioned, ``block`` where elements are put into
smaller arrays from continuous blocks of original array(number of
smaller arrays is defined by ``factor``) and ``complete`` where array is
decomposed into individual elements each having own read/write ports.

.. code:: cpp

   #pragma HLS ARRAY_PARTITION variable = A dim = 1 cyclic factor = 16
   #pragma HLS ARRAY_PARTITION variable = B dim = 1 block factor = 16

Following is the log reported while running the design on U200 platform:

::

   Platform Name: Xilinx
   INFO: Reading ./build_dir.hw.xilinx_u200_xdma_201830_2/matmul.xclbin
   Loading: './build_dir.hw.xilinx_u200_xdma_201830_2/matmul.xclbin'
   Trying to program device[0]: xilinx_u200_xdma_201830_2
   Device[0]: program successful!
   |-------------------------+-------------------------|
   | Kernel(100 iterations) | Wall-Clock Time (ns) |
   |-------------------------+-------------------------|
   | matmul: naive | 12976188 |
   | matmul: partition | 10423378 |
   |-------------------------+-------------------------|
   Note: Wall Clock Time is meaningful for real hardware execution only, not for emulation.
   Please refer to profile summary for kernel execution time for hardware emulation.
   TEST PASSED

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.