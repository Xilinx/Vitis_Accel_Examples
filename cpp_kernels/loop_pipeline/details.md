Loop Pipelining
================

This example demonstrates how loop pipelining can be used to improve the performance of a kernel. Pipelining is a technique to increase instruction level parallelism in the hardware implementation of an algorithm by overlapping independent stages of operations and functions.

Two kernels `kernel_vadd` and `kernel_pipelined` are used for comparison in performance. Kernel execution time will be less for the pipelined kernel as it uses `pragma HLS PIPELINE` in read and write loops which enables `burst transfers` resulting in effective usage of bandwidth of `m_axi` in interface.

```c++
read_a:
        for (int x = 0; x < N; ++x) {
           #pragma HLS LOOP_TRIPCOUNT min=c_n max=c_n
           #pragma HLS PIPELINE II=1
            result[x] = a[i * N + x];
        }
```
