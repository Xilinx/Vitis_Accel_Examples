Array Block and Cyclic Partitioning
===================================

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
