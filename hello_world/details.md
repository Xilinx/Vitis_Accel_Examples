Hello World
=============
This example introduces the basic structure of host and kernel code. Kernel vadd optimizes a simple vector addition.


Kernel arguments specify the inputs read from the global memory and outputs written back to it.
HLS interface pragma is used to specify the protocol used for transfer of data between global memory and the kernel and creation of
RTL ports during interface synthesis.

```c++
#pragma HLS INTERFACE m_axi port=in1  offset=slave bundle=gmem
```

If the number of loop iterations are unknown, tool cannot report the loop latency. In these cases, HLS LOOP_TRIPCOUNT pragma can be used to specify minimum and
maximum number of iterations to help the tool to analyze how the loop's latency contributes to the total latency of the application.

```c++
#pragma HLS LOOP_TRIPCOUNT min=c_len max=c_len
```

Loop iterations are done sequentially on a CPU i.e. it will take multiple clock cycles for a new iteration to begin execution. FPGA can pipeline these iterations
to increase the throughput of the application by beginning a new iteration every cycle. HLS PIPELINE pragma is used to implement pipeline and `option II` can be
used to mention the Initiation interval (No. of clock cycles before a new loop iteration can be started).


```c++
#pragma HLS PIPELINE II=1
```

Rather than reading individual items for addition, buffers are created in kernel's local memory and multiple items are read in a single burst.
This is done to achieve low memory access latency and also for efficient use of bandwidth provided by the m_axi interface.
Similarly, results are stored in a buffer and are written to global memory in a burst.
The for loops used have the following requirements to implement burst read/write:
- Pipeline the loop : Loop pipeline must have II(Initiation interval) = 1
- Aligned memory : Memory addresses for read/write should be contiguous.

```c++
    read1: for (int j = 0 ; j < chunk_size ; j++){
        #pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
        #pragma HLS PIPELINE II=1
            v1_buffer[j] = in1[i + j];
        }
    write: for (int j = 0 ; j < chunk_size ; j++){
        #pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
        #pragma HLS PIPELINE II=1
            out_r[i + j] = vout_buffer[j];
            }    
```
