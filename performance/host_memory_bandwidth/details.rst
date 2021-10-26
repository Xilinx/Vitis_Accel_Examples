Host Memory Bandwidth(C)
========================

This is a host application to test host memory bandwidth. It contains 3 kernels namely read_bandwidth, write_bandwidth and bandwidth (for concurrent read and write).The host application runs all 3 kernels with variable buffer sizes and measures the bandwidth for each case. Finally the design reports the maximum bandwidth achieved for each case.

The host allocates a buffer into specific host-only buffer using ``XCL_MEM_EXT_HOST_ONLY``. The ``cl_mem_ext_ptr`` object needs to be used in cases where memory assignment is done by user explicitly:

.. code:: cpp

   cl_mem_ext_ptr_t input_buffer_ext;
   input_buffer_ext.flags = XCL_MEM_EXT_HOST_ONLY;
   input_buffer_ext.obj = nullptr;
   input_buffer_ext.param = 0;
   
   OCL_CHECK(err, buffer[0] = new cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, bufsize,
                                             &input_buffer_ext, &err));

Using the ``sp`` option  in the krnl_bandwidth.cfg file, AXI-Master Port is connected to the Slave-Bridge IP:

::

   [connectivity]
   sp=bandwidth_1.input0:HOST[0]
   sp=bandwidth_1.output0:HOST[0]
   sp=read_bandwidth_1.input0:HOST[0]
   sp=write_bandwidth_1.output0:HOST[0]

Following is the real log reported while running the design on U250 platform:

::

   Found Platform
   Platform Name: Xilinx
   INFO: Reading ./build_dir.hw.xilinx_u250_gen3x16_xdma_3_1_202020_1/bandwidth.xclbin
   Loading: './build_dir.hw.xilinx_u250_gen3x16_xdma_3_1_202020_1/bandwidth.xclbin'
   Trying to program device[0]: xilinx_u250_gen3x16_xdma_shell_3_1
   Device[0]: program successful!
   Concurrent Read and Write Throughput = 6.96748 (GB/sec) for buffer size 4.00 KB
   Read Throughput = 4.87813 (GB/sec) for buffer size 4.00 KB
   Write Throughput = 4.69212 (GB/sec) for buffer size 4.00 KB

   Concurrent Read and Write Throughput = 7.25228 (GB/sec) for buffer size 8.00 KB
   Read Throughput = 4.95737 (GB/sec) for buffer size 8.00 KB
   Write Throughput = 5.155 (GB/sec) for buffer size 8.00 KB

   Concurrent Read and Write Throughput = 8.31089 (GB/sec) for buffer size 16.00 KB
   Read Throughput = 6.0383 (GB/sec) for buffer size 16.00 KB
   Write Throughput = 6.16268 (GB/sec) for buffer size 16.00 KB
   
   Concurrent Read and Write Throughput = 12.1802 (GB/sec) for buffer size 32.00 KB
   Read Throughput = 8.17289 (GB/sec) for buffer size 32.00 KB
   Write Throughput = 8.26587 (GB/sec) for buffer size 32.00 KB

   Concurrent Read and Write Throughput = 15.9402 (GB/sec) for buffer size 64.00 KB
   Read Throughput = 9.62244 (GB/sec) for buffer size 64.00 KB
   Write Throughput = 9.59521 (GB/sec) for buffer size 64.00 KB
   
   Concurrent Read and Write Throughput = 19.0601 (GB/sec) for buffer size 128.00 KB
   Read Throughput = 10.6148 (GB/sec) for buffer size 128.00 KB
   Write Throughput = 10.7014 (GB/sec) for buffer size 128.00 KB
   
   Concurrent Read and Write Throughput = 20.8453 (GB/sec) for buffer size 256.00 KB
   Read Throughput = 10.4822 (GB/sec) for buffer size 256.00 KB
   Write Throughput = 11.2627 (GB/sec) for buffer size 256.00 KB

   Concurrent Read and Write Throughput = 21.8754 (GB/sec) for buffer size 512.00 KB
   Read Throughput = 10.3349 (GB/sec) for buffer size 512.00 KB
   Write Throughput = 11.5728 (GB/sec) for buffer size 512.00 KB

   Concurrent Read and Write Throughput = 22.9009 (GB/sec) for buffer size 1.00 MB
   Read Throughput = 11.4337 (GB/sec) for buffer size 1.00 MB
   Write Throughput = 11.7316 (GB/sec) for buffer size 1.00 MB

   Concurrent Read and Write Throughput = 23.3553 (GB/sec) for buffer size 2.00 MB
   Read Throughput = 11.6192 (GB/sec) for buffer size 2.00 MB
   Write Throughput = 11.8272 (GB/sec) for buffer size 2.00 MB

   Concurrent Read and Write Throughput = 21.2646 (GB/sec) for buffer size 4.00 MB
   Read Throughput = 11.6355 (GB/sec) for buffer size 4.00 MB
   Write Throughput = 11.8706 (GB/sec) for buffer size 4.00 MB

   Concurrent Read and Write Throughput = 19.8527 (GB/sec) for buffer size 8.00 MB
   Read Throughput = 11.0061 (GB/sec) for buffer size 8.00 MB
   Write Throughput = 11.8895 (GB/sec) for buffer size 8.00 MB

   Concurrent Read and Write Throughput = 14.4411 (GB/sec) for buffer size 16.00 MB
   Read Throughput = 10.688 (GB/sec) for buffer size 16.00 MB
   Write Throughput = 11.6946 (GB/sec) for buffer size 16.00 MB

   Concurrent Read and Write Throughput = 13.0544 (GB/sec) for buffer size 32.00 MB
   Read Throughput = 10.6945 (GB/sec) for buffer size 32.00 MB
   Write Throughput = 11.4721 (GB/sec) for buffer size 32.00 MB
   
   Concurrent Read and Write Throughput = 11.4484 (GB/sec) for buffer size 64.00 MB
   Read Throughput = 10.6969 (GB/sec) for buffer size 64.00 MB
   Write Throughput = 10.6954 (GB/sec) for buffer size 64.00 MB
   
   Concurrent Read and Write Throughput = 11.7352 (GB/sec) for buffer size 128.00 MB
   Read Throughput = 10.6941 (GB/sec) for buffer size 128.00 MB
   Write Throughput = 10.5788 (GB/sec) for buffer size 128.00 MB

   Concurrent Read and Write Throughput = 11.59 (GB/sec) for buffer size 256.00 MB
   Read Throughput = 10.7008 (GB/sec) for buffer size 256.00 MB
   Write Throughput = 10.5752 (GB/sec) for buffer size 256.00 MB

   Maximum bandwidth achieved :
   Concurrent Read and Write Throughput = 23.3553 (GB/sec) 
   Read Throughput = 11.6355 (GB/sec) 
   Write Throughput = 11.8895 (GB/sec) 

   TEST PASSED
