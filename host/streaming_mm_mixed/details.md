Stream Memory Map Mixed
===============================================================

This example demonstrate how to process an input stream of data from host for computation along with memory mapped OpenCL buffer inputs. `krnl_stream_vadd` has 1 stream input, one memory mapped buffer input and 1 stream output.

Following is the kernel definition along with Interface pragma:

```c++
typedef qdma_axis<DWIDTH, 0, 0, 0> pkt;
void krnl_stream_vadd(hls::stream<pkt> &out, // Write-Only output Stream
                      hls::stream<pkt> &in1, // Input Stream 1
                      int *in2,              // Memory mapped Input Vector 2
                      int size               // Size in integer
) {
#pragma HLS INTERFACE axis port = out
#pragma HLS INTERFACE axis port = in1
#pragma HLS INTERFACE m_axi port = in2 offset = slave bundle = gmem
#pragma HLS INTERFACE s_axilite port = in2 bundle = control
#pragma HLS INTERFACE s_axilite port = size bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control
...
}
```
Here kernel has two input argument in1 and in2. in1 is axis (AXI Streaming) type and in2 is m_axi (memory Master AXI) Interface type.

Inside host code, user has to set argument only for non-streaming interface as shown below:
```c++
cl::Kernel krnl_vadd = cl::Kernel(program, "krnl_stream_vadd", &err));
krnl_vadd.setArg(2, buffer_in2);
krnl_vadd.setArg(3, size);
```

For `axis` streaming interface type argument, host needs to creates streaming objects as below:

```c++
cl_int ret;
cl_mem_ext_ptr_t ext;
ext.param = krnl_vadd.get(); //specifying the kernel object on which stream connection will be established
ext.obj = NULL;

ext.flags = 0; //Create read stream for argument 0 of kernel
cl_stream read_stream xcl::Stream::createStream(device.get(), CL_STREAM_READ_ONLY, CL_STREAM, &ext, &ret));
ext.flags = 1; //Create write stream for argument 1 
cl_stream write_stream_a = xcl::Stream::createStream(device.get(), CL_STREAM_WRITE_ONLY, CL_STREAM, &ext, &ret));
```

Please refer `streaming_simple` example to more about Streaming kernel usage.
