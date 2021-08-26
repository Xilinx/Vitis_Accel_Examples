Array Partition
===============

This example demonstrates how ``array partition`` in HLS kernel can help
to improve the performance. In this example matrix multiplication
functionality is used to showcase the benefit of array partition. Design
contains two kernels “matmul” a simple matrix multiplication and
“matmul_partition” a matrix multiplication implementation using array
partition.

``#pragma HLS array partition`` is used to partition an array into
multiple smaller arrays or memories. Arrays can be partitioned in three
ways, ``cyclic``, ``block`` and ``complete``. In this example,
``complete`` partition is used to partition one of the dimension of
local Matrix array as below

.. code:: cpp

   int B[MAX_SIZE][MAX_SIZE];
   int C[MAX_SIZE][MAX_SIZE];
   #pragma HLS ARRAY_PARTITION variable = B dim = 2 complete
   #pragma HLS ARRAY_PARTITION variable = C dim = 2 complete

This array partition helps design to access 2nd dimension of both Matrix
B and C concurrently to reduce the overall latency.

To see the benefit of array partition, user can look into system
estimate report and see overall latency. Latency Information of normal
matmul kernel (without partition):

::

   Compute Unit  Kernel Name  Module Name  Start Interval  Best (cycles)  Avg (cycles)  Worst (cycles)  Best (absolute)  Avg (absolute)  Worst (absolute)
   ------------  -----------  -----------  --------------  -------------  ------------  --------------  ---------------  --------------  ----------------
   matmul_1      matmul       matmul       2068 ~ 3052     2067           2559          3051            6.889 us         8.529 us        9.526 us

Latency Information for matrix multiplication for kernel with partition:

::

   Compute Unit        Kernel Name       Module Name       Start Interval  Best (cycles)  Avg (cycles)  Worst (cycles)  Best (absolute)  Avg (absolute)  Worst (absolute)
   ------------------  ----------------  ----------------  --------------  -------------  ------------  --------------  ---------------  --------------  ----------------
   matmul_partition_1  matmul_partition  matmul_partition  277 ~ 1260      276            768           1259            0.920 us         2.560 us        4.196 us

Example generates the following information as output when ran on Alevo
U250 Card:

::

   Found Platform
   Platform Name: Xilinx
   INFO: Reading ./build_dir.hw.xilinx_u250_gen3x16_xdma_3_1_202020_1/matmul.xclbin 
   Loading: './build_dir.hw.xilinx_u250_gen3x16_xdma_3_1_202020_1/matmul.xclbin'
   |-------------------------+-------------------------|
   | Kernel                  |    Wall-Clock Time (ns) |
   |-------------------------+-------------------------|
   | matmul:                 |                    6826 |
   | matmul: partition       |                     853 |
   |-------------------------+-------------------------|
   | Speedup                 |                8.002345 |
   |-------------------------+-------------------------|
   Note: Wall Clock Time is meaningful for real hardware execution only, not for emulation.
   Please refer to profile summary for kernel execution time for hardware emulation.
   TEST PASSED
