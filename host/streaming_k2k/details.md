Streaming Kernel to Kernel (Vector addition and multiplication)
=============================================================
This example demonstrates how two kernels `kernel_stream_vadd` and `kernel_stream_vmult` can communicate through a stream interface.
Rather than loading the results from producer kernel `kernel_stream_vadd` to global memory and then to consumer kernel `kernel_stream_vmult` we can use stream to transfer this data directly. It reduces the latency of the application and also saves the memory resources for other operations.

For kernel to kernel streaming transfer, kernel to kernel data transfer ports don't need `clSetKernelArg` from host code.

Producer kernel streaming output port needs to be connected to consumer kernel stream input port during `xocc` linking stage.

`xocc -l --sc krnl_stream_vadd_1.output:krnl_stream_vmult_1.b`


Streaming class needs to be initialized before use.
```c++
xcl::Stream::init(platform_id);
```
For stream, Device connection to the kernel is specified by using `cl_mem_ext_ptr_t` extention pointer.
```c++
cl_mem_ext_ptr_t ext;
ext.param = krnl_adder1.get();
ext.obj = NULL;
```

`xcl::stream::createStream` API is used to create a stream and read and write properties are determined by the flags `CL_STREAM_WRITE_ONLY` and `CL_STREAM_READ_ONLY` . `.flags` is used to specify the kernel argument to which stream is connected.

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
For non-blocking stream, other operations can be carried out while data is being written into or being read from the stream. Non-blocking stream requires `CL_STREAM_NONBLOCKING` flag to be specified in the transfer initiation request.

```c++
nb_wr_req.flags = CL_STREAM_EOT | CL_STREAM_NONBLOCKING;
```

 Since non-blocking streams are asynchronous and return immediately, `xcl::stream::pollStream` is a blocking API used to monitor the status of completion of the transfer through streams, it returns the execution to the host code after streams are completed.

```c++
    cl_streams_poll_req_completions poll_req[4]{0, 0, 0, 0};
    auto num_compl = 4;
    xcl::Stream::pollStreams(device.get(), poll_req, 4, 4, &num_compl, 50000, &ret);
  ```  

`xcl::clreleaseStream` is used to free streaming resources and the memory associated with them.
```c++
    xcl::Stream::releaseStream(read_stream);
    xcl::Stream::releaseStream(write_stream_a);
```

Host to kernel streams require the datatype `qdma_axis<D,0,0,0>` and intra-kernel streaming interface should be defined by hls::stream with the `ap_axiu<D,0,0,0>` datatype. These datatypes need the header file `ap_axi_sdata.h`.

```c++
typedef qdma_axis<DWIDTH, 0, 0, 0> pkt;
typedef ap_axiu<DWIDTH, 0, 0, 0> trans_pkt;
```

 These classes have variables `data`,`last` and `keep` to manage the data transfer.

`data`: Internally qdma_axis datatype has `ap_uint<D>` which can be accessed by `get_data()` and `set_data()` methods.

`keep`: For all data before last, `keep` variable must be set to `-1` to denote all bytes of data are valid. For the last data, the kernel has the flexibility to send fewer bytes. For example, for the four bytes data transfer, the kernel can truncate the last data by sending 1 byte or 2 bytes or 3 bytes by using set_keep() function.  

```c++
      t_out.set_data(tmpOut);
      t_out.set_last(t1.get_last());
      t_out.set_keep(-1);
```

`last`: Final data transferred must be identified by the last variable. get_last() and set_last() methods are used to access/set the last variable. Kernel doesn't know how many data items are coming through the stream. Stream is polled by calling get_last() after every transfer and breaks when get_last() returns 1.

