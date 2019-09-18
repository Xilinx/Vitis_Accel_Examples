Streaming Host bandwidth 
=========================

This example tests the bandwidth of blocking and non-blocking stream interface between host and device. 

To measure the maximum Bandwidth a wider stream width is used at the kernel level as below:

```c++
#include "ap_axi_sdata.h"
typedef qdma_axis<512, 0, 0, 0> pkt;
void krnl_stream_adder1(hls::stream<pkt> &a, hls::stream<pkt> &output) {
    #pragma HLS INTERFACE axis port=a
    #pragma HLS INTERFACE axis port=output
    #pragma HLS INTERFACE s_axilite port=return bundle=control
...
}
```

Kernel is enqueued with event object to measure the kernel execution time as below:
```c++
cl::Event nb_wait_event;
q.enqueueTask(krnl_adder1, NULL, &nb_wait_event);
...
q.finish(); // wait for all pending enqueue
unsigned long start, stop;
wait_event.getProfilingInfo<unsigned long>(CL_PROFILING_COMMAND_START, &start));
wait_event.getProfilingInfo<unsigned long>(CL_PROFILING_COMMAND_END, &stop));
unsigned long duration = stop - start;
```


