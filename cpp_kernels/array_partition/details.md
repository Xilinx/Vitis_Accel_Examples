Array Partition
================
This example demonstrates how `array partition` in HLS kernels can improve the performance of an application. Operations like matrix multiplication require multiple accesses to data in memory for a single iteration.

`#pragma HLS array partition` is used to partition an array into multiple smaller arrays or memories with more number of ports for read and write operations. This results in improved throughput of the design.

Arrays can be partitioned in three ways, `cyclic` where elements are put into smaller arrays one by one in the interleaved manner until the whole array is partitioned, `block` where elements are put into smaller arrays from continuous blocks of original array and `complete` where array is decomposed into individual elements each having its own read/write ports.

```c++
#pragma HLS ARRAY_PARTITION variable = B dim = 2 complete
#pragma HLS ARRAY_PARTITION variable = C dim = 2 complete
#pragma HLS ARRAY_PARTITION variable = temp_sum dim = 1 complete
```    
