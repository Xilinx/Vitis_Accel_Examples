Stream Simple
=============

This is Simple Streaming Kernel Example and demonstrate how host
application can directly transfer data via streaming interface without
moving data to Global memory. Host Application demonstrate both
approaches (blocking and non-blocking) stream interface between host and
device. Stream interface does not require address management as access
to data is done in sequential manner. Stream interfaces are useful for
applications where either the data is too big to reside on an FPGA or
data is being streamed from a sensor network.

Inside Kernel code HLS pragma must be defined for every streaming
interface.

.. code:: cpp

   #include "ap_axi_sdata.h"
   typedef qdma_axis<32, 0, 0, 0> pkt;
   void krnl_stream_adder1(hls::stream<pkt> &a, hls::stream<pkt> &output) {
       #pragma HLS INTERFACE axis port=a
       #pragma HLS INTERFACE axis port=output
       #pragma HLS INTERFACE s_axilite port=return bundle=control
   ...
   }

``hls::stream`` kernels use a special class ``qdma_axis<D,0,0,0>`` for
kernel streams which requires the header file ``ap_axi_sdata.h``. It has
variables ``data``,\ ``last`` and ``keep`` to manage the data transfer.

``data``: Internally qdma_axis datatype has ``ap_uint<D>`` which can be
accessed by ``get_data()`` and ``set_data()`` methods.

``keep``: For all data before last, ``keep`` variable must be set to
``-1`` to denote all bytes of data are valid. For the last data, the
kernel has the flexibility to send fewer bytes. For example, for the
four bytes data transfer, the kernel can truncate the last data by
sending 1 byte or 2 bytes or 3 bytes by using set_keep() function.

.. code:: cpp

   typedef qdma_axis<32, 0, 0, 0> t_out;
   t_out.set_data(tmpOut);
   t_out.set_last(t1.get_last());
   t_out.set_keep(-1);

``last``: Final data transferred must be identified by the last
variable. get_last() and set_last() methods are used to access/set the
last variable. Kernel doesn not know how many data items are coming
through the stream. Stream is polled by calling get_last() after every
transfer and breaks when get_last() returns 1.

Host to Kernel Streaming is supported by all QDMA based platforms. To
use streaming support and utility APIs provided inside xcl2.hpp,
programmer needs to specify following in host code:

.. code:: cpp

   // Declaration of custom stream APIs that binds to Xilinx Streaming APIs.
   decltype(&clCreateStream) xcl::Stream::createStream = nullptr;
   decltype(&clReleaseStream) xcl::Stream::releaseStream = nullptr;
   decltype(&clReadStream) xcl::Stream::readStream = nullptr;
   decltype(&clWriteStream) xcl::Stream::writeStream = nullptr;
   decltype(&clPollStreams) xcl::Stream::pollStreams = nullptr;

Streaming class needs to be initialized before use as below:

.. code:: cpp

   xcl::Stream::init(platform_id);

To make a streaming object connection to specific kernel of design,
following steps are needed:

.. code:: cpp

   cl_mem_ext_ptr_t ext;
   ext.param = krnl_adder1.get();
   ext.obj = NULL;
   ext.flags = 0; // Indicates that connect to argument 0 of kernel
   cl_stream write_stream_a = xcl::Stream::createStream(device.get(), XCL_STREAM_READ_ONLY, CL_STREAM, &ext,nullptr));
   ext.flags = 1; // Indicates that connect to argument 1 of kernel
   cl_stream read_stream = xcl::Stream::createStream(device.get(), XCL_STREAM_WRITE_ONLY, CL_STREAM, &ext, &ret));

``xcl::stream::createStream`` API is used to create a stream and read
and write properties are determined by the flags
``XCL_STREAM_WRITE_ONLY`` and ``XCL_STREAM_READ_ONLY`` .\ ``.flags`` is
used to specify the kernel argument to which stream is connected.

There are blocking and non-blocking APIs to transfer data between Host
and kernel via stream interface.

Blocking stream requires the stream operation (read or write) to finish
before the next operation can be executed. Following shows a blocking
call of writeStream but inside a child thread:

.. code:: cpp

   cl_stream_xfer_req b_wr_req{0};
   b_wr_req.flags = CL_STREAM_EOT;
   b_wr_req.priv_data = (void *)"b_write_a";
   // Thread 1 for writing data to input stream 1 independently in case of default blocking transfers.
   std::thread thr1(xcl::Stream::writeStream, 
                   write_stream_a,     // cl_stream object
                   h_a.data(),         // host memory pointer from where the data has to be transferred
                   vector_size_bytes,  // size of data to be transfered in bytes
                   &b_wr_req,          // xfer req flag to indicate type of transfer
                   &ret); 

Similar to this following shows a blocking call of readStream but inside
a child thread:

.. code:: cpp

   cl_stream_xfer_req b_rd_req{0};
   b_rd_req.flags = CL_STREAM_EOT;
   b_rd_req.priv_data = (void *)"b_read_out";
   // Output thread to read the stream data independently in case of default blocking transfers.
   std::thread thr2(xcl::Stream::readStream,
                    read_stream,       //cl_stream object
                    hw_results.data(), // host memory pointer on which data will be read
                    vector_size_bytes, // max size of data which can be stored in host memory
                    &b_rd_req,         // xfer_req flag to indicate type of transfer
                    &ret);

As both are blocking calls running inside different threads. So host
application needs to wait for thread to finish to successful data
transfer using thread ``join()`` API as below:

``xcl::clReadstream`` and ``xcl::clWritestream`` APIs are used to read
from and write to streams respectively.

.. code:: cpp

   thr1.join();
   thr2.join();

In case of non-blocking stream, other operations can be carried out
while data is being written into or being read from the stream.
Non-blocking stream requires ``CL_STREAM_NONBLOCKING`` flag to be
specified in the transfer initiation request.

.. code:: cpp

   cl_stream_xfer_req nb_wr_req{0};
   nb_wr_req.flags = CL_STREAM_EOT | CL_STREAM_NONBLOCKING;
   nb_wr_req.priv_data = (void *)"nb_write_a";
   xcl::Stream::writeStream(write_stream_a, h_a.data(), vector_size_bytes, &nb_wr_req, &ret));
   cl_stream_xfer_req nb_rd_req{0};
   nb_rd_req.flags = CL_STREAM_EOT | CL_STREAM_NONBLOCKING;
   nb_rd_req.priv_data = (void *)"nb_read";
   xcl::Stream::readStream(read_stream, hw_results.data(),vector_size_bytes,&nb_rd_req,&ret));

Since non-blocking streams are asynchronous and return immediately,
``xcl::stream::pollStream`` is a blocking API used to monitor the status
of completion of the transfer through streams, it returns the execution
to the host code after streams are completed.

.. code:: cpp

   cl_streams_poll_req_completions poll_req[2]{0, 0}; // 2 Requests
   auto num_compl = 2;
   xcl::Stream::pollStreams(device.get(), poll_req, 2, 2, &num_compl, 50000, &ret);
   // Blocking API, waits for 2 poll request completion or 50000ms, whichever occurs first.

``xcl::clreleaseStream`` is used to release stream objects.

.. code:: cpp

   xcl::Stream::releaseStream(read_stream);
   xcl::Stream::releaseStream(write_stream_a);
