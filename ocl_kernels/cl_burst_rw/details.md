Burst Read/write
================

The usual reason for having a burst mode capability, or using burst mode, is to increase data throughput. This example demonstrates how multiple items can be read from global memory to kernel's local memory in a single burst.
This is done to achieve low memory access latency and also for efficient use of bandwidth provided by the `AXI` interface. 
Similarly, computation results are stored in a buffer and are written to global memory in a burst.

```c++
__attribute__((xcl_pipeline_loop(1)))
__attribute__((xcl_loop_tripcount(c_size, c_size)))
        read_buf: for (int j = 0; j < chunk_size; j++) {
            burstbuffer[j] = a[i+j]
```
The for loops used have the following requirements to implement burst read/write:

- Pipeline the loop : Loop pipeline must have II(Initiation interval) = 1 specfied by the pipeline pragma inside the loop.
- Aligned memory : Memory addresses for read/write should be contiguous.
