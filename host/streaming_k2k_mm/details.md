Streaming Kernel to Kernel(Vector addtion and multiplication)
==============================================================

This example demonstrates how kernels can have memory mapped inputs along with stream interface from one kernel to another.

`kernel_stream_vadd` has two memory mapped inputs and one stream output. `kernel_stream_vmult` has one memory mapped input along with kernel to kernel stream from `kernel_stream_vadd` which acts as its second input.

Kernel arguments are not required to be setup in host code for kernel to kernel streaming interfaces. Argument 2 for kernel_vadd and argument 1 for kernel_vmult are not declared.  

```c++
err = krnl_vadd.setArg(0, buffer_in1);
err = krnl_vadd.setArg(1, buffer_in2);
err = krnl_vadd.setArg(3, size));
err = krnl_vmult.setArg(0, buffer_in3));
err = krnl_vmult.setArg(2, buffer_output));
err = krnl_vmult.setArg(3, size));
```

Since this example employs only a kernel to kernel stream, no stream APIs are required in the host code.

HLS pragma must be defined for every streaming interface.
```c++
#pragma HLS INTERFACE axis port=out
```
`hls::stream` kernels use a special class `ap_axiu<D,0,0,0>` for intra-kernel streams  which requires the header file `ap_axi_sdata.h`. It has variables `data`,`last` and `keep` to manage the data transfer.

