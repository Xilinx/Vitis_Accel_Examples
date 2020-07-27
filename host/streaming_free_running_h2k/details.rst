Streaming Host to Kernel
========================

This example demonstrates how host and kernel can communicate through a
stream interface. Rather than loading the data from host to global
memory and then to consumer kernel ``increment`` we can use stream to
transfer this data directly. It reduces the latency of the application
and also reduces the memory utilization. This is a simple increment
Kernel design in which streams of kernel are directly communicating with
host using stream interfaces.

For free running kernel, user needs to specify ap_ctrl_none for return
port. This will create the kernel without AXI lite interface. Kernel
will always be in running states.

.. code:: cpp

   #pragma HLS interface ap_ctrl_none port=return

Following is the log reported while running the design on U200 platform:

::

   Platform Name: Xilinx
   INFO: Reading increment.xclbin
   Loading: 'increment.xclbin'
   Trying to program device[0]: xilinx_u200_qdma_201920_1
   Device[0]: program successful!

    Writing Stream h2c_stream_a[0]
    Reading Stream c2h_stream[0]
    Writing Stream h2c_stream_a[1]
    Reading Stream c2h_stream[1]
    Writing Stream h2c_stream_a[2]
    Reading Stream c2h_stream[2]
    Writing Stream h2c_stream_a[3]
    Reading Stream c2h_stream[3]
    Writing Stream h2c_stream_a[4]
    Reading Stream c2h_stream[4]
    Writing Stream h2c_stream_a[5]
    Reading Stream c2h_stream[5]
    Writing Stream h2c_stream_a[6]
    Reading Stream c2h_stream[6]
    Writing Stream h2c_stream_a[7]
    Reading Stream c2h_stream[7]
    Writing Stream h2c_stream_a[8]
    Reading Stream c2h_stream[8]
    Writing Stream h2c_stream_a[9]
    Reading Stream c2h_stream[9]
    Writing Stream h2c_stream_a[10]
    Reading Stream c2h_stream[10]
    Writing Stream h2c_stream_a[11]
    Reading Stream c2h_stream[11]
    Writing Stream h2c_stream_a[12]
    Reading Stream c2h_stream[12]
    Writing Stream h2c_stream_a[13]
    Reading Stream c2h_stream[13]
    Writing Stream h2c_stream_a[14]
    Reading Stream c2h_stream[14]
    Writing Stream h2c_stream_a[15]
    Reading Stream c2h_stream[15]
    Writing Stream h2c_stream_a[16]
    Reading Stream c2h_stream[16]
    Writing Stream h2c_stream_a[17]
    Reading Stream c2h_stream[17]
    Writing Stream h2c_stream_a[18]
    Reading Stream c2h_stream[18]
    Writing Stream h2c_stream_a[19]
    Reading Stream c2h_stream[19]
    Writing Stream h2c_stream_a[20]
    Reading Stream c2h_stream[20]
    Writing Stream h2c_stream_a[21]
    Reading Stream c2h_stream[21]
    Writing Stream h2c_stream_a[22]
    Reading Stream c2h_stream[22]
    Writing Stream h2c_stream_a[23]
    Reading Stream c2h_stream[23]
    Writing Stream h2c_stream_a[24]
    Reading Stream c2h_stream[24]
    Writing Stream h2c_stream_a[25]
    Reading Stream c2h_stream[25]
    Writing Stream h2c_stream_a[26]
    Reading Stream c2h_stream[26]
    Writing Stream h2c_stream_a[27]
    Reading Stream c2h_stream[27]
    Writing Stream h2c_stream_a[28]
    Reading Stream c2h_stream[28]
    Writing Stream h2c_stream_a[29]
    Reading Stream c2h_stream[29]
    Writing Stream h2c_stream_a[30]
    Reading Stream c2h_stream[30]
    Writing Stream h2c_stream_a[31]
    Reading Stream c2h_stream[31]
    clPollStreams for (64) events (CU: 32, axis_in: 1, axis_out: 1)
   [ Case: 1 ] -> Throughput = 2.38802 GB/s
   TEST PASSED 
