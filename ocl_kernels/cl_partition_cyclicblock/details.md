Array Block and Cyclic Partitioning
====================================
This example demonstrates how to use array `block` and `cyclic` partitioning to improve the performance of the kernel.
Matrix multiplication is performed in this example which would require repeated access to rows of former matrix and columns of latter. 
Array can be partitioned across different dimensions to reduce the latency of these transfers.

`xcl_array_partition` attribute is used to partition an array into multiple smaller arrays with more number of ports for read and write 
operations. This results in improved throughput of the design.

Arrays can be partitioned in three ways, `cyclic` where elements are put into smaller arrays one by one in the interleaved manner
untill the whole array is partitioned, `block` where elements are put into smaller arrays from continuous blocks of original array
and `complete` where array is decomposed into individual elements each having own read/write ports.

```c++
 int A[MAX_DIM * MAX_DIM] __attribute__((xcl_array_partition(cyclic, MAX_DIM, 1)));
 int B[MAX_DIM * MAX_DIM] __attribute__((xcl_array_partition(block, MAX_DIM, 1)));
```
