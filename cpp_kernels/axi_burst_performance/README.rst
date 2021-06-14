AXI Burst Performance
=====================

This is an AXI Burst Performance check design. It measures the time it takes to write a buffer into DDR or read a buffer from DDR. The example contains 2 sets of 6 kernels each: each set having a different data width and each kernel having a different burst_length and num_outstanding parameters to compare the impact of these parameters on effective throughput.

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   zcu102_base_20
   zcu104_base_20
   vck
   zc7
   aws-vu9p-f1
   samsung
   nodma

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/test_kernel_common.hpp
   src/test_kernel_maxi_256bit_1.cpp
   src/test_kernel_maxi_256bit_2.cpp
   src/test_kernel_maxi_256bit_3.cpp
   src/test_kernel_maxi_256bit_4.cpp
   src/test_kernel_maxi_256bit_5.cpp
   src/test_kernel_maxi_256bit_6.cpp
   src/test_kernel_maxi_512bit_1.cpp
   src/test_kernel_maxi_512bit_2.cpp
   src/test_kernel_maxi_512bit_3.cpp
   src/test_kernel_maxi_512bit_4.cpp
   src/test_kernel_maxi_512bit_5.cpp
   src/test_kernel_maxi_512bit_6.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./axi_burst_performance -x1 <test_kernel_maxi_256bit XCLBIN> -x2 <test_kernel_maxi_512bit XCLBIN>

DETAILS
-------

This is an AXI Burst Performance check design. It measures the time it takes to write a buffer into DDR or read a buffer from DDR. The example contains 2 sets of 6 kernels each: each set having a different data width and each kernel having a different burst_length and num_outstanding parameters to compare the impact of these parameters on effective throughput.

A counter is coded inside each of the kernels to accurately count the number of cycles between the start and end of the buffer transfer.

In this version, the kernels are configured as follows:

::

   Data Width - 256
   test_kernel_maxi_256bit_1: burst length= 4, outstanding transactions=4
   test_kernel_maxi_256bit_2: burst length=16, outstanding transactions=4
   test_kernel_maxi_256bit_3: burst length=32, outstanding transactions=4
   test_kernel_maxi_256bit_4: burst length= 4, outstanding transactions=32
   test_kernel_maxi_256bit_5: burst length=16, outstanding transactions=32
   test_kernel_maxi_256bit_6: burst length=32, outstanding transactions=32
   
   Data Width - 512
   test_kernel_maxi_512bit_1: burst length= 4, outstanding transactions=4
   test_kernel_maxi_512bit_2: burst length=16, outstanding transactions=4
   test_kernel_maxi_512bit_3: burst length=32, outstanding transactions=4
   test_kernel_maxi_512bit_4: burst length= 4, outstanding transactions=32
   test_kernel_maxi_512bit_5: burst length=16, outstanding transactions=32
   test_kernel_maxi_512bit_6: burst length=32, outstanding transactions=32

Below are the resource numbers while running the design on U200 platform:

Data Width - 256

========================= ==== ==== ====
Kernel                    LUT  REG  BRAM
========================= ==== ==== ====
test_kernel_maxi_256bit_1 4.2K 7.2K 11  
test_kernel_maxi_256bit_2 4.3K 7.2K 11  
test_kernel_maxi_256bit_3 4.4K 7.3K 11  
test_kernel_maxi_256bit_4 4.3K 7.2K 11  
test_kernel_maxi_256bit_5 4.3K 7.3K 11  
test_kernel_maxi_256bit_6 4.5K 7.1K 15  
========================= ==== ==== ====

Data Width - 512

========================= ==== ==== ====
Kernel                    LUT  REG  BRAM
========================= ==== ==== ====
test_kernel_maxi_512bit_1 4.8K 9.0K 14  
test_kernel_maxi_512bit_2 4.9K 9.1K 14  
test_kernel_maxi_512bit_3 5.2K 9.1K 14  
test_kernel_maxi_512bit_4 4.9K 9.1K 14  
test_kernel_maxi_512bit_5 4.9K 9.1K 14  
test_kernel_maxi_512bit_6 5.2K 9.0K 23  
========================= ==== ==== ====

Following is the real log reported while running the design on U200 platform for 16 KB transfers:

::

   Test parameters
    - xclbin file   : ./build_dir.hw.xilinx_u200_xdma_201830_2/test_kernel_maxi_256bit.xclbin
    - frequency     : 300 MHz
    - buffer size   : 16.00 KB
   
   Found Platform
   Platform Name: Xilinx
   INFO: Reading ./build_dir.hw.xilinx_u200_xdma_201830_2/test_kernel_maxi_256bit.xclbin
   Loading: './build_dir.hw.xilinx_u200_xdma_201830_2/test_kernel_maxi_256bit.xclbin'
   Trying to program device[1]: xilinx_u200_xdma_201830_2
   Device[1]: program successful!
   
   Kernel->AXI Burst WRITE performance
   Data Width = 256 burst_length = 4 num_outstanding = 4 buffer_size = 16.00 KB | throughput = 2.55877 GB/sec
   Data Width = 256 burst_length = 16 num_outstanding = 4 buffer_size = 16.00 KB | throughput = 6.31398 GB/sec
   Data Width = 256 burst_length = 32 num_outstanding = 4 buffer_size = 16.00 KB | throughput = 6.84251 GB/sec
   Data Width = 256 burst_length = 4 num_outstanding = 32 buffer_size = 16.00 KB | throughput = 4.26223 GB/sec
   Data Width = 256 burst_length = 16 num_outstanding = 32 buffer_size = 16.00 KB | throughput = 6.45647 GB/sec
   Data Width = 256 burst_length = 32 num_outstanding = 32 buffer_size = 16.00 KB | throughput = 6.84251 GB/sec
   
   Kernel->AXI Burst READ performance
   Data Width = 256 burst_length = 4 num_outstanding = 4 buffer_size = 16.00 KB | throughput = 2.01658 GB/sec
   Data Width = 256 burst_length = 16 num_outstanding = 4 buffer_size = 16.00 KB | throughput = 6.54884 GB/sec
   Data Width = 256 burst_length = 32 num_outstanding = 4 buffer_size = 16.00 KB | throughput = 7.79836 GB/sec
   Data Width = 256 burst_length = 4 num_outstanding = 32 buffer_size = 16.00 KB | throughput = 7.7851 GB/sec
   Data Width = 256 burst_length = 16 num_outstanding = 32 buffer_size = 16.00 KB | throughput = 7.79836 GB/sec
   Data Width = 256 burst_length = 32 num_outstanding = 32 buffer_size = 16.00 KB | throughput = 7.79836 GB/sec
   
   Test parameters
    - xclbin file   : ./build_dir.hw.xilinx_u200_xdma_201830_2/test_kernel_maxi_512bit.xclbin
    - frequency     : 300 MHz
    - buffer size   : 16.00 KB
   
   Found Platform
   Platform Name: Xilinx
   INFO: Reading ./build_dir.hw.xilinx_u200_xdma_201830_2/test_kernel_maxi_512bit.xclbin
   Loading: './build_dir.hw.xilinx_u200_xdma_201830_2/test_kernel_maxi_512bit.xclbin'
   Trying to program device[1]: xilinx_u200_xdma_201830_2
   Device[1]: program successful!
   
   Kernel->AXI Burst WRITE performance
   Data Width = 512 burst_length = 4 num_outstanding = 4 buffer_size = 16.00 KB | throughput = 5.17832 GB/sec
   Data Width = 512 burst_length = 16 num_outstanding = 4 buffer_size = 16.00 KB | throughput = 8.23316 GB/sec
   Data Width = 512 burst_length = 32 num_outstanding = 4 buffer_size = 16.00 KB | throughput = 11.5306 GB/sec
   Data Width = 512 burst_length = 4 num_outstanding = 32 buffer_size = 16.00 KB | throughput = 8.10201 GB/sec
   Data Width = 512 burst_length = 16 num_outstanding = 32 buffer_size = 16.00 KB | throughput = 11.5016 GB/sec
   Data Width = 512 burst_length = 32 num_outstanding = 32 buffer_size = 16.00 KB | throughput = 11.2473 GB/sec
   
   Kernel->AXI Burst READ performance
   Data Width = 512 burst_length = 4 num_outstanding = 4 buffer_size = 16.00 KB | throughput = 4.04385 GB/sec
   Data Width = 512 burst_length = 16 num_outstanding = 4 buffer_size = 16.00 KB | throughput = 11.6776 GB/sec
   Data Width = 512 burst_length = 32 num_outstanding = 4 buffer_size = 16.00 KB | throughput = 13.6646 GB/sec
   Data Width = 512 burst_length = 4 num_outstanding = 32 buffer_size = 16.00 KB | throughput = 13.6646 GB/sec
   Data Width = 512 burst_length = 16 num_outstanding = 32 buffer_size = 16.00 KB | throughput = 13.6646 GB/sec
   Data Width = 512 burst_length = 32 num_outstanding = 32 buffer_size = 16.00 KB | throughput = 13.6646 GB/sec
   
   TEST PASSED

Following is the real log reported while running the design on U200 platform for 16 MB transfers:

::

   Test parameters
    - xclbin file   : ./build_dir.hw.xilinx_u200_xdma_201830_2/test_kernel_maxi_256bit.xclbin
    - frequency     : 300 MHz
    - buffer size   : 16.00 MB
   
   Found Platform
   Platform Name: Xilinx
   INFO: Reading ./build_dir.hw.xilinx_u200_xdma_201830_2/test_kernel_maxi_256bit.xclbin
   Loading: './build_dir.hw.xilinx_u200_xdma_201830_2/test_kernel_maxi_256bit.xclbin'
   Trying to program device[1]: xilinx_u200_xdma_201830_2
   Device[1]: program successful!
   
   Kernel->AXI Burst WRITE performance
   Data Width = 256 burst_length = 4 num_outstanding = 4 buffer_size = 16.00 MB | throughput = 2.66919 GB/sec
   Data Width = 256 burst_length = 16 num_outstanding = 4 buffer_size = 16.00 MB | throughput = 6.62449 GB/sec
   Data Width = 256 burst_length = 32 num_outstanding = 4 buffer_size = 16.00 MB | throughput = 7.59737 GB/sec
   Data Width = 256 burst_length = 4 num_outstanding = 32 buffer_size = 16.00 MB | throughput = 4.47013 GB/sec
   Data Width = 256 burst_length = 16 num_outstanding = 32 buffer_size = 16.00 MB | throughput = 7.1518 GB/sec
   Data Width = 256 burst_length = 32 num_outstanding = 32 buffer_size = 16.00 MB | throughput = 7.94597 GB/sec
   
   Kernel->AXI Burst READ performance
   Data Width = 256 burst_length = 4 num_outstanding = 4 buffer_size = 16.00 MB | throughput = 2.02206 GB/sec
   Data Width = 256 burst_length = 16 num_outstanding = 4 buffer_size = 16.00 MB | throughput = 6.80909 GB/sec
   Data Width = 256 burst_length = 32 num_outstanding = 4 buffer_size = 16.00 MB | throughput = 8.59958 GB/sec
   Data Width = 256 burst_length = 4 num_outstanding = 32 buffer_size = 16.00 MB | throughput = 8.68773 GB/sec
   Data Width = 256 burst_length = 16 num_outstanding = 32 buffer_size = 16.00 MB | throughput = 8.93942 GB/sec
   Data Width = 256 burst_length = 32 num_outstanding = 32 buffer_size = 16.00 MB | throughput = 8.93942 GB/sec
   
   Test parameters
    - xclbin file   : ./build_dir.hw.xilinx_u200_xdma_201830_2/test_kernel_maxi_512bit.xclbin
    - frequency     : 300 MHz
    - buffer size   : 16.00 MB
   
   Found Platform
   Platform Name: Xilinx
   INFO: Reading ./build_dir.hw.xilinx_u200_xdma_201830_2/test_kernel_maxi_512bit.xclbin
   Loading: './build_dir.hw.xilinx_u200_xdma_201830_2/test_kernel_maxi_512bit.xclbin'
   Trying to program device[1]: xilinx_u200_xdma_201830_2
   Device[1]: program successful!
   
   Kernel->AXI Burst WRITE performance
   Data Width = 512 burst_length = 4 num_outstanding = 4 buffer_size = 16.00 MB | throughput = 5.1399 GB/sec
   Data Width = 512 burst_length = 16 num_outstanding = 4 buffer_size = 16.00 MB | throughput = 11.7942 GB/sec
   Data Width = 512 burst_length = 32 num_outstanding = 4 buffer_size = 16.00 MB | throughput = 14.6941 GB/sec
   Data Width = 512 burst_length = 4 num_outstanding = 32 buffer_size = 16.00 MB | throughput = 8.93979 GB/sec
   Data Width = 512 burst_length = 16 num_outstanding = 32 buffer_size = 16.00 MB | throughput = 14.3008 GB/sec
   Data Width = 512 burst_length = 32 num_outstanding = 32 buffer_size = 16.00 MB | throughput = 15.1586 GB/sec
   
   Kernel->AXI Burst READ performance
   Data Width = 512 burst_length = 4 num_outstanding = 4 buffer_size = 16.00 MB | throughput = 3.92988 GB/sec
   Data Width = 512 burst_length = 16 num_outstanding = 4 buffer_size = 16.00 MB | throughput = 13.1114 GB/sec
   Data Width = 512 burst_length = 32 num_outstanding = 4 buffer_size = 16.00 MB | throughput = 16.8218 GB/sec
   Data Width = 512 burst_length = 4 num_outstanding = 32 buffer_size = 16.00 MB | throughput = 16.8222 GB/sec
   Data Width = 512 burst_length = 16 num_outstanding = 32 buffer_size = 16.00 MB | throughput = 16.8295 GB/sec
   Data Width = 512 burst_length = 32 num_outstanding = 32 buffer_size = 16.00 MB | throughput = 16.8219 GB/sec
   
   TEST PASSED

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.