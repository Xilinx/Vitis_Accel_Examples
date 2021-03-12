Slave Bridge Bandwidth XRT (XRT Native API's)
=============================================

This is a host application to test Slavebridge bandwidth. It contains 3 kernels namely read_bandwidth, write_bandwidth and bandwidth (for concurrent read and write).The host application runs all 3 kernels with variable buffer sizes and measures the bandwidth for each case. Finally the design reports the maximum bandwidth achieved for each case.

The host allocates a buffer into specific host-only buffer using ``host_only`` flag.

.. code:: cpp

   xrt::bo::flags flags = xrt::bo::flags::host_only;
   auto hostonly_bo_in = xrt::bo(device, bufsize, flags, krnl.group_id(0));
   auto hostonly_bo_out = xrt::bo(device, bufsize, flags, krnl.group_id(1));

Using the ``sp`` option  in the krnl_bandwidth.cfg file, AXI-Master Port is connected to the Slave-Bridge IP:

::

   sp=bandwidth_1.input0:HOST[0]
   sp=bandwidth_1.output0:HOST[0]
   sp=read_bandwidth_1.input0:HOST[0]
   sp=write_bandwidth_1.output0:HOST[0]

Following is the real log reported while running the design on U250 platform:

::

    Open the device0
    Load the xclbin ./build_dir.hw.xilinx_u250_gen3x16_xdma_3_1_202020_1/bandwidth.xclbin
    Concurrent Read and Write Throughput = 7.48369 (GB/sec) for buffer size 4.00 KB
    Read Throughput = 0.000335063 (GB/sec) for buffer size 4.00 KB
    Write Throughput = 0.000344473 (GB/sec) for buffer size 4.00 KB

    Concurrent Read and Write Throughput = 7.33095 (GB/sec) for buffer size 8.00 KB
    Read Throughput = 0.000665857 (GB/sec) for buffer size 8.00 KB
    Write Throughput = 0.000693077 (GB/sec) for buffer size 8.00 KB

    Concurrent Read and Write Throughput = 8.49993 (GB/sec) for buffer size 16.00 KB
    Read Throughput = 0.00154943 (GB/sec) for buffer size 16.00 KB
    Write Throughput = 0.0015804 (GB/sec) for buffer size 16.00 KB

    Concurrent Read and Write Throughput = 12.4713 (GB/sec) for buffer size 32.00 KB
    Read Throughput = 0.00397313 (GB/sec) for buffer size 32.00 KB
    Write Throughput = 0.0040426 (GB/sec) for buffer size 32.00 KB

    Concurrent Read and Write Throughput = 15.8589 (GB/sec) for buffer size 64.00 KB
    Read Throughput = 0.00929847 (GB/sec) for buffer size 64.00 KB
    Write Throughput = 0.00944232 (GB/sec) for buffer size 64.00 KB

    Concurrent Read and Write Throughput = 18.7322 (GB/sec) for buffer size 128.00 KB
    Read Throughput = 0.0202977 (GB/sec) for buffer size 128.00 KB
    Write Throughput = 0.0203858 (GB/sec) for buffer size 128.00 KB

    Concurrent Read and Write Throughput = 20.2429 (GB/sec) for buffer size 256.00 KB
    Read Throughput = 0.0397041 (GB/sec) for buffer size 256.00 KB
    Write Throughput = 0.0429825 (GB/sec) for buffer size 256.00 KB

    Concurrent Read and Write Throughput = 21.1149 (GB/sec) for buffer size 512.00 KB
    Read Throughput = 0.0783758 (GB/sec) for buffer size 512.00 KB
    Write Throughput = 0.0869292 (GB/sec) for buffer size 512.00 KB

    Concurrent Read and Write Throughput = 21.7278 (GB/sec) for buffer size 1.00 MB
    Read Throughput = 0.156576 (GB/sec) for buffer size 1.00 MB
    Write Throughput = 0.177042 (GB/sec) for buffer size 1.00 MB

    Concurrent Read and Write Throughput = 21.8723 (GB/sec) for buffer size 2.00 MB
    Read Throughput = 0.30943 (GB/sec) for buffer size 2.00 MB
    Write Throughput = 0.356215 (GB/sec) for buffer size 2.00 MB

    Concurrent Read and Write Throughput = 22.0848 (GB/sec) for buffer size 4.00 MB
    Read Throughput = 0.615448 (GB/sec) for buffer size 4.00 MB
    Write Throughput = 0.720843 (GB/sec) for buffer size 4.00 MB

    Concurrent Read and Write Throughput = 22.4699 (GB/sec) for buffer size 8.00 MB
    Read Throughput = 1.26089 (GB/sec) for buffer size 8.00 MB
    Write Throughput = 1.4473 (GB/sec) for buffer size 8.00 MB

    Concurrent Read and Write Throughput = 14.6714 (GB/sec) for buffer size 16.00 MB
    Read Throughput = 2.71928 (GB/sec) for buffer size 16.00 MB
    Write Throughput = 2.84091 (GB/sec) for buffer size 16.00 MB

    Concurrent Read and Write Throughput = 13.6195 (GB/sec) for buffer size 32.00 MB
    Read Throughput = 5.44235 (GB/sec) for buffer size 32.00 MB
    Write Throughput = 5.74132 (GB/sec) for buffer size 32.00 MB
    
    Concurrent Read and Write Throughput = 12.6072 (GB/sec) for buffer size 64.00 MB
    Read Throughput = 11.0016 (GB/sec) for buffer size 64.00 MB
    Write Throughput = 11.3533 (GB/sec) for buffer size 64.00 MB
    
    Maximum bandwidth achieved :
    Concurrent Read and Write Throughput = 22.4699 (GB/sec) 
    Read Throughput = 11.0016 (GB/sec) 
    Write Throughput = 11.3533 (GB/sec) 
    
    TEST PASSED
