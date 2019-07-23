Burst Read/write
================

This example demonstrates how multiple items can be read from global memory to kernel's local memory in a single burst.
This is done to achieve low memory access latency and also for efficient use of bandwidth provided by the `m_axi` interface. 
Similarly, computation results are stored in a buffer and are written to global memory in a burst.

`#pragma HLS PIPELINE II=1`

The for loops used have the following requirements to implement burst read/write:

- Pipeline the loop : Loop pipeline must have II(Initiation interval) = 1
- Aligned memory : Memory addresses for read/write should be contiguous.
