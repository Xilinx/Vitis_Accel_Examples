Host Global Bandwidth
=====================

This example tests the data transfer bandwidth between host and global
memory for 3 cases : host to global memory transfer, global memory to
host transfer and bidirectional transfer. For PCIe platform case, Host
and device are connected via PCIe interconnect and latency of this
interconnect can be seen as the acceleration tax for the application.
Hence, these transfers should be efficiently managed.

For bandwidth test, ``clenqueueMigrateMemObjects()`` API is used to
transfer the data from host to global memory and vice versa.

.. code:: cpp

   err = commands.enqueueMigrateMemObjects(mems1, 0/* 0 means from host*/);
   err = commands.enqueueMigrateMemObjects(mems2, CL_MIGRATE_MEM_OBJECT_HOST);

Following is the real log reported while running the design on U200
platform:

::

   Platform Name: Xilinx
   Found Device=xilinx_u200_xdma_201830_1
   INFO: Reading ./build_dir.hw.xilinx_u200_xdma_201830_1/krnl_host_global.xclbin
   Loading: './build_dir.hw.xilinx_u200_xdma_201830_1/krnl_host_global.xclbin'
   OpenCL migration BW host to device: 4.70066 MB/s for buffer size 0.0625 KB with 1024 buffers
   OpenCL migration BW device to host: 4.91932 MB/s for buffer size 0.0625 KB with 1024 buffers
   OpenCL migration BW host to device: 19.9712 MB/s for buffer size 0.25 KB with 1024 buffers
   OpenCL migration BW device to host: 20.0787 MB/s for buffer size 0.25 KB with 1024 buffers
   OpenCL migration BW host to device: 37.3776 MB/s for buffer size 0.5 KB with 1024 buffers
   OpenCL migration BW device to host: 39.6291 MB/s for buffer size 0.5 KB with 1024 buffers
   OpenCL migration BW host to device: 68.5495 MB/s for buffer size 1 KB with 1024 buffers
   OpenCL migration BW device to host: 76.4584 MB/s for buffer size 1 KB with 1024 buffers
   OpenCL migration BW host to device: 294.746 MB/s for buffer size 4 KB with 1024 buffers
   OpenCL migration BW device to host: 313.529 MB/s for buffer size 4 KB with 1024 buffers
   OpenCL migration BW host to device: 1169.25 MB/s for buffer size 16 KB with 512 buffers
   OpenCL migration BW device to host: 1114.67 MB/s for buffer size 16 KB with 512 buffers
   OpenCL migration BW host to device: 6488.24 MB/s for buffer size 1024 KB with 8 buffers
   OpenCL migration BW device to host: 9732.36 MB/s for buffer size 1024 KB with 8 buffers
   OpenCL migration BW host to device: 6619.78 MB/s for buffer size 1024 KB with 64 buffers
   OpenCL migration BW device to host: 10072.4 MB/s for buffer size 1024 KB with 64 buffers
   OpenCL migration BW host to device: 6793.51 MB/s for buffer size 1024 KB with 256 buffers
   OpenCL migration BW device to host: 11194.7 MB/s for buffer size 1024 KB with 256 buffers
   OpenCL migration BW host to device: 6808.51 MB/s for buffer size 2048 KB with 8 buffers
   OpenCL migration BW device to host: 10376.1 MB/s for buffer size 2048 KB with 8 buffers
   OpenCL migration BW host to device: 7713.63 MB/s for buffer size 2048 KB with 64 buffers
   OpenCL migration BW device to host: 11583.7 MB/s for buffer size 2048 KB with 64 buffers
   OpenCL migration BW host to device: 7621.88 MB/s for buffer size 2048 KB with 256 buffers
   OpenCL migration BW device to host: 11527.4 MB/s for buffer size 2048 KB with 256 buffers
   OpenCL migration BW host to device: 7233.73 MB/s for buffer size 16384 KB with 64 buffers
   OpenCL migration BW device to host: 11455.5 MB/s for buffer size 16384 KB with 64 buffers
   OpenCL migration BW host to device: 7432.3 MB/s for buffer size 262144 KB with 4 buffers
   OpenCL migration BW device to host: 12112 MB/s for buffer size 262144 KB with 4 buffers
   OpenCL migration BW host to device: 7452.04 MB/s for buffer size 524288 KB with 2 buffers
   OpenCL migration BW device to host: 12128.5 MB/s for buffer size 524288 KB with 2 buffers

   The bandwidth numbers for bidirectional case:
   OpenCL migration BW overall:8.46482 MB/s for buffer size 0.0625 KB with 1024 buffers
   OpenCL migration BW overall:33.3023 MB/s for buffer size 0.25 KB with 1024 buffers
   OpenCL migration BW overall:66.912 MB/s for buffer size 0.5 KB with 1024 buffers
   OpenCL migration BW overall:129.132 MB/s for buffer size 1 KB with 1024 buffers
   OpenCL migration BW overall:524.969 MB/s for buffer size 4 KB with 1024 buffers
   OpenCL migration BW overall:1990.79 MB/s for buffer size 16 KB with 512 buffers
   OpenCL migration BW overall:13039.9 MB/s for buffer size 1024 KB with 8 buffers
   OpenCL migration BW overall:14733 MB/s for buffer size 1024 KB with 64 buffers
   OpenCL migration BW overall:15262.2 MB/s for buffer size 1024 KB with 256 buffers
   OpenCL migration BW overall:14065.9 MB/s for buffer size 2048 KB with 8 buffers
   OpenCL migration BW overall:14935 MB/s for buffer size 2048 KB with 64 buffers
   OpenCL migration BW overall:15399.2 MB/s for buffer size 2048 KB with 256 buffers
   OpenCL migration BW overall:14626.7 MB/s for buffer size 16384 KB with 64 buffers
   OpenCL migration BW overall:14904.3 MB/s for buffer size 262144 KB with 4 buffers
   OpenCL migration BW overall:14906.7 MB/s for buffer size 524288 KB with 2 buffers

   TEST PASSED
