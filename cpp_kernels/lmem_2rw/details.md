Local Memory Two Parallel Read/Write
========================================
This is a simple example to demonstrate how to utilize both ports of local memory in kernels.

Kernel's local memory is usually `BRAM` which has two ports for read/write.
In loops where one iteration doesn't depend on previous iterations, two ports can be used to improve the performance of the kernel.

Two ports can be utilized concurrently by using `pragma HLS UNROLL`. The UNROLL pragma transform loops by creating multiples copies of the loop body
in the register transfer level (RTL) design, which allows some or all loop iterations to occur in parallel.
```c++
 #pragma HLS UNROLL FACTOR=2
 ```
Here loop is unrolled by a factor of 2 thus two iterations of the loop are executed concurrently. 
In this case, two ports of BRAM will be utilized rather than 1 reducing the total loop latency by half approximately.
```c++
 vadd:
        for (int j = 0; j < chunk_size; j++) {
           #pragma HLS PIPELINE II=1
           #pragma HLS UNROLL FACTOR=2
           #pragma HLS LOOP_TRIPCOUNT min=c_chunk_sz max=c_chunk_sz
            //perform vector addition
            vout_buffer[j] = v1_buffer[j] + v2_buffer[j];
        }
```
