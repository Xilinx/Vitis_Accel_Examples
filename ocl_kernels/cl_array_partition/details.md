Array Partition
================
This example demonstrates how `array partition` in OpenCL kernels can improve the performance of an application. Operations like matrix
multiplication require multiple accesses to data in memory for a single iteration. Two kernels are executed, `matmul_partition` with the array partitions and 
`matmul` without the array partitions and performance is compared.

`xcl_array_partition` is used to partition an array into multiple smaller arrays or memories with more number of ports for read and 
write operations. This results in improved throughput of the design.

Arrays can be partitioned in three ways, `cyclic` where elements are put into smaller arrays one by one in the interleaved manner 
until the whole array is partitioned, `block` where elements are put into smaller arrays from continuous blocks of original array and 
`complete` where array is decomposed into individual elements each having its own read/write ports.

```c++
    int B[MAX_SIZE][MAX_SIZE] __attribute__((xcl_array_partition(complete, 2)));
    
    int C[MAX_SIZE][MAX_SIZE] __attribute__((xcl_array_partition(complete, 2)));

    int temp_sum[MAX_SIZE]    __attribute__((xcl_array_partition(complete, 1)));
```
