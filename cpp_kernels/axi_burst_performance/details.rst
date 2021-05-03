AXI Burst Performance
=====================

This design measures the time it takes to write a buffer into DDR or read a buffer from DDR. The example contains 6 kernels, each with different burst_length and num_outstanding parameters to compare the impact of these parameters on effective throughput.

A counter is coded inside each of the kernels to accurately count the number of cycles between the start and end of the buffer transfer.

In this version, the kernels are configured as follows:

::

   testKernel_1: burst length= 4, outstanding transactions=4
   testKernel_2: burst length=16, outstanding transactions=4
   testKernel_3: burst length=32, outstanding transactions=4
   testKernel_4: burst length= 4, outstanding transactions=32
   testKernel_5: burst length=16, outstanding transactions=32
   testKernel_6: burst length=32, outstanding transactions=32

Below are the resource numbers while running the design on U200 platform:

============ ==== ==== ====
Kernel       LUT  REG  BRAM
============ ==== ==== ====
testKernel_1 4847 9036 14  
testKernel_2 4895 9061 14  
testKernel_3 5154 9060 14  
testKernel_4 4910 9073 14  
testKernel_5 4942 9085 14  
testKernel_6 5179 9013 23  
============ ==== ==== ====

Following is the real log reported while running the design on U200 platform for 16 KB transfers:

::

   Test parameters
    - xclbin file   : ./build_dir.hw.xilinx_u200_xdma_201830_2/testKernel.xclbin
    - frequency     : 300 Mhz
    - buffer size   : 16.00 KB
   
   Found Platform
   Platform Name: Xilinx
   INFO: Reading ./build_dir.hw.xilinx_u200_xdma_201830_2/testKernel.xclbin
   Loading: './build_dir.hw.xilinx_u200_xdma_201830_2/testKernel.xclbin'
   Trying to program device[1]: xilinx_u200_xdma_201830_2
   Device[1]: program successful!
   
   Kernel->AXI Burst WRITE performance
   burst_length = 4 num_outstanding = 4 buffer_size = 16.00 KB | throughput = 4.61455 GB/sec
   burst_length = 16 num_outstanding = 4 buffer_size = 16.00 KB | throughput = 10.3801 GB/sec
   burst_length = 32 num_outstanding = 4 buffer_size = 16.00 KB | throughput = 8.41477 GB/sec
   burst_length = 4 num_outstanding = 32 buffer_size = 16.00 KB | throughput = 8.10201 GB/sec
   burst_length = 16 num_outstanding = 32 buffer_size = 16.00 KB | throughput = 11.5306 GB/sec
   burst_length = 32 num_outstanding = 32 buffer_size = 16.00 KB | throughput = 10.9775 GB/sec
   
   Kernel->AXI Burst READ performance
   burst_length = 4 num_outstanding = 4 buffer_size = 16.00 KB | throughput = 3.77693 GB/sec
   burst_length = 16 num_outstanding = 4 buffer_size = 16.00 KB | throughput = 11.6776 GB/sec
   burst_length = 32 num_outstanding = 4 buffer_size = 16.00 KB | throughput = 9.8869 GB/sec
   burst_length = 4 num_outstanding = 32 buffer_size = 16.00 KB | throughput = 13.6646 GB/sec
   burst_length = 16 num_outstanding = 32 buffer_size = 16.00 KB | throughput = 13.4636 GB/sec
   burst_length = 32 num_outstanding = 32 buffer_size = 16.00 KB | throughput = 13.6646 GB/sec
   
   TEST PASSED

Following is the real log reported while running the design on U200 platform for 16 MB transfers:

::

   Test parameters
    - xclbin file   : ./build_dir.hw.xilinx_u200_xdma_201830_2/testKernel.xclbin
    - frequency     : 300 Mhz
    - buffer size   : 16.00 MB
   
   Found Platform
   Platform Name: Xilinx
   INFO: Reading ./build_dir.hw.xilinx_u200_xdma_201830_2/testKernel.xclbin
   Loading: './build_dir.hw.xilinx_u200_xdma_201830_2/testKernel.xclbin'
   Trying to program device[1]: xilinx_u200_xdma_201830_2
   Device[1]: program successful!
   
   Kernel->AXI Burst WRITE performance
   burst_length = 4 num_outstanding = 4 buffer_size = 16.00 MB | throughput = 5.13962 GB/sec
   burst_length = 16 num_outstanding = 4 buffer_size = 16.00 MB | throughput = 11.7986 GB/sec
   burst_length = 32 num_outstanding = 4 buffer_size = 16.00 MB | throughput = 14.6955 GB/sec
   burst_length = 4 num_outstanding = 32 buffer_size = 16.00 MB | throughput = 8.93887 GB/sec
   burst_length = 16 num_outstanding = 32 buffer_size = 16.00 MB | throughput = 14.2987 GB/sec
   burst_length = 32 num_outstanding = 32 buffer_size = 16.00 MB | throughput = 15.1677 GB/sec
   
   Kernel->AXI Burst READ performance
   burst_length = 4 num_outstanding = 4 buffer_size = 16.00 MB | throughput = 3.93185 GB/sec
   burst_length = 16 num_outstanding = 4 buffer_size = 16.00 MB | throughput = 13.1127 GB/sec
   burst_length = 32 num_outstanding = 4 buffer_size = 16.00 MB | throughput = 16.8296 GB/sec
   burst_length = 4 num_outstanding = 32 buffer_size = 16.00 MB | throughput = 16.8294 GB/sec
   burst_length = 16 num_outstanding = 32 buffer_size = 16.00 MB | throughput = 16.8294 GB/sec
   burst_length = 32 num_outstanding = 32 buffer_size = 16.00 MB | throughput = 16.8295 GB/sec
   
   TEST PASSED
