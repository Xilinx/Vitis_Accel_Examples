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

Following is the log reported while running the design on U250 platform:

::

   Found Platform
   Platform Name: Xilinx
   INFO: Reading ./build_dir.hw.xilinx_u250_gen3x16_xdma_3_1_202020_1/matmul.xclbin
   Loading: './build_dir.hw.xilinx_u250_gen3x16_xdma_3_1_202020_1/matmul.xclbin'
   Trying to program device[0]: xilinx_u250_gen3x16_xdma_shell_3_1
   Device[0]: program successful!
   |-------------------------+-------------------------|
   | Kernel                  |    Wall-Clock Time (ns) |
   |-------------------------+-------------------------|
   | matmul: naive           |                   20496 |
   | matmul: partition       |                     866 |
   |-------------------------+-------------------------|
   | Speedup                 |                  23.664 |
   |-------------------------+-------------------------|
   Note: Wall Clock Time is meaningful for real hardware execution only, not for emulation.
   Please refer to profile summary for kernel execution time for hardware emulation.
   TEST PASSED
