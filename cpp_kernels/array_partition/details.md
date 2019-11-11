Array Partition
================
This example demonstrates how `array partition` in HLS kernel can help to improve the performance. In this example matrix multiplication functionality is used to showcase the benefit of array partition. Design contains two kernels "matmul" a simple matrix multiplication and "matmul_partition" a matrix multiplication implementation using array partition.

`#pragma HLS array partition` is used to partition an array into multiple smaller arrays or memories. Arrays can be partitioned in three ways, `cyclic`, `block` and `complete`. In this example, `complete` partition is used to partition one of the dimension of local Matrix array as below

```c++
int B[MAX_SIZE][MAX_SIZE];
int C[MAX_SIZE][MAX_SIZE];
#pragma HLS ARRAY_PARTITION variable = B dim = 2 complete
#pragma HLS ARRAY_PARTITION variable = C dim = 2 complete
```    

This array partition helps design to access 2nd dimension of both Matrix B and C concurrently to reduce the overall latency.

To see the benefit of array partition, user can look into system estimate report and see overall latency. 
Latency Information of normal matmul kernel (without partition):
```
Compute Unit  Kernel Name  Module Name  Start Interval  Best (cycles)  Avg (cycles)  Worst (cycles)  Best (absolute)  Avg (absolute)  Worst (absolute)
------------  -----------  -----------  --------------  -------------  ------------  --------------  ---------------  --------------  ----------------
matmul_1      matmul       matmul       2856 ~ 2859     2855           2857          2858            9.516 us         9.522 us        9.526 us
```
Latency Information for matrix multiplication for kernel with partition:
```
Compute Unit        Kernel Name       Module Name       Start Interval  Best (cycles)  Avg (cycles)  Worst (cycles)  Best (absolute)  Avg (absolute)  Worst (absolute)
------------------  ----------------  ----------------  --------------  -------------  ------------  --------------  ---------------  --------------  ----------------
matmul_partition_1  matmul_partition  matmul_partition  1063 ~ 1066     1062           1064          1065            3.540 us         3.546 us        3.550 us
```

Example generates the following information as output when ran on Alevo U200 Card:
```
Found Platform
Platform Name: Xilinx
INFO: Reading ./build_dir.hw.xilinx_u200_qdma_201910_1/matmul.xclbin
Loading: './build_dir.hw.xilinx_u200_qdma_201910_1/matmul.xclbin'
|-------------------------+-------------------------|
| Kernel                  |    Wall-Clock Time (ns) |
|-------------------------+-------------------------|
| matmul:                 |                  396685 |
| matmul: partition       |                  256367 |
|-------------------------+-------------------------|
Note: Wall Clock Time is meaningful for real hardware execution only, not for emulation.
Please refer to profile summary for kernel execution time for hardware emulation.
TEST PASSED
```
