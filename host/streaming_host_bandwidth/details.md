Streaming Host to Kernel Bandwidth 
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

Following is the log reported while running the design on U200 platform:
```
Platform Name: Xilinx
INFO: Reading ./build_dir.hw.xilinx_u200_qdma_201910_1/krnl_stream_adder1.xclbin
Loading: './build_dir.hw.xilinx_u200_qdma_201910_1/krnl_stream_adder1.xclbin'
############################################################
Blocking Stream
############################################################
[ Case: 1 ] -> Throughput = 5.06 GB/s
TEST PASSED
############################################################
Non-Blocking Stream
############################################################
[ Case: 2 ] -> Throughput = 5.25 GB/s
TEST PASSED
```
