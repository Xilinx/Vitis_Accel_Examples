Stream Vector addition with OpenCL Buffers
============================================

This example demontrates how to process an input stream of data from host for computation along with memory mapped OpenCL buffer inputs. `krnl_stream_vadd` has 1 stream input, one memory mapped buffer input and 1 stream output.

The APIs used for streaming operations are custom stream APIs that binds to Xilinx Streaming APIs.

Streaming class needs to be initialized before use.
```c++
xcl::Stream::init(platform_id);
```
For stream, Device connection to the kernel is specified by using `cl_mem_ext_ptr_t`extention pointer.
```c++
cl_mem_ext_ptr_t ext;
ext.param = krnl_vadd.get();
ext.obj = NULL;
```

`xcl::stream::createStream` API is used to create a stream and read and write properties are determined by the flags `CL_STREAM_WRITE_ONLY` and `CL_STREAM_READ_ONLY` .`.flags` is used to specify the kernel argument to which stream is connected.

```c++
    cl_stream read_stream;
    ext.flags = 1;
    read_stream = xcl::Stream::createStream(device.get(), CL_STREAM_READ_ONLY, CL_STREAM, &ext, &ret);
```    
Transfer initiation for reading and writing to streams requires declaration of following atribute. Successful stream transfer  depends upon determining the end of transfer signal, indicated by `CL_STREAM_EOT` flag.
```c++
    cl_stream_xfer_req b_wr_req{0};
    b_wr_req.flags = CL_STREAM_EOT;
```      


`xcl::clReadstream` and `xcl::clWritestream` APIs are used to read from and write to streams respectively.

```c++
  xcl::Stream::writeStream(write_stream_a, h_a.data(), vector_size_bytes, &b_wr_req, &ret);
```

`xcl::clreleaseStream` is used to free streaming resources and the memory associated with them.
```c++
    xcl::Stream::releaseStream(read_stream);
    xcl::Stream::releaseStream(write_stream_a);
```

HLS pragma must be defined for every streaming interface.
```c++
#pragma HLS INTERFACE axis port=out
#pragma HLS INTERFACE axis port=in1
```

`hls::stream` kernels use a special class `qdma_axis<D,0,0,0>` for host to kernel streams  which requires the header file `ap_axi_sdata.h`. It has variables `data`,`last` and `keep` to manage the data transfer.

`data`: Internally qdma_axis datatype has `ap_uint<D>` which can be accessed by `get_data()` and `set_data()` methods.

`keep`: For all data before last, `keep` variable must be set to `-1` to denote all bytes of data are valid. For the last data, the kernel has the flexibility to send fewer bytes. For example, for the four bytes data transfer, the kernel can truncate the last data by sending 1 byte or 2 bytes or 3 bytes by using set_keep() function.  

```c++
  out_buf.set_data(res);
  out_buf.set_keep(-1);
  out_buf.set_last(in_buf.get_last());

```

`last`: Final data transferred must be identified by the last variable. `get_last()` and `set_last()` methods are used to access/set the last variable. Kernel doesn't know how many data items are coming through the stream. Stream is polled by calling `get_last()` after every transfer and breaks when `get_last()` returns 1.


