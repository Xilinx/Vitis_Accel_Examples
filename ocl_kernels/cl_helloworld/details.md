Hello World
=============
This example introduces the basic structure of host and kernel code. Kernel `vadd` optimizes a simple vector addition.


Kernel arguments with `global` keyword specify the inputs read from the global memory and outputs written back to it.

If the number of loop iterations are unknown, tool cannot report the loop latency. In these cases, `xcl_loop_tripcount` can be used to specify minimum and
maximum number of iterations to help the tool to analyze how the loop's latency contributes to the total latency of the application.

```c++
 __attribute__((xcl_loop_tripcount(c_len, c_len)))
```

Loop iterations are done sequentially on a CPU, i.e. it will take multiple clock cycles for a new iteration to begin execution. FPGA
can pipeline these iterations to increase the throughput of the application by beginning a new iteration every cycle. `xcl_pipeline_loop` attribute
is used to implement pipeline and to mention the *Initiation interval*
(No. of clock cycles before a new loop iteration can be started).


```c++
__attribute__((xcl_pipeline_loop(1)))
```

Rather than reading individual items for addition, buffers are created in kernel's local memory and multiple items are read in a single
burst. This is done to achieve low memory access latency and also for efficient use of bandwidth provided by the `AXI` interface.
Similarly, results are stored in a buffer and are written to global memory in a burst.
The for loops used have the following requirements to implement burst read/write:
- Pipeline the loop : Loop pipeline must have `II` (Initiation interval) = 1
- Aligned memory : Memory addresses for read/write should be contiguous.

```c++
   __attribute__((xcl_pipeline_loop(1)))
        readB: for (int j = 0 ; j < size ; j++) {
                arrayB[j] = b[i+j]; }
```
