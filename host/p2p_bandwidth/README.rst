P2P bandwidth Example
=====================

This is simple example to test data transfer between SSD and FPGA.

**KEY CONCEPTS:** `P2P <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/p2p>`__, SmartSSD, XDMA

**KEYWORDS:** XCL_MEM_EXT_P2P_BUFFER, pread, pwrite

**EXCLUDED PLATFORMS:** 

 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - All Versal Platforms, i.e vck190 etc
 - AWS VU9P F1
 - All Platforms with 201910 Version
 - Alveo U25 SmartNIC
 - Alveo U30
 - Alveo U50 gen3x16 xdma 2019 Version

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   data/sample.txt
   src/bandwidth.cpp
   src/host.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./p2p_bandwidth -x <bandwidth XCLBIN> -f ./data/sample.txt

DETAILS
-------

PCIe peer-to-peer communication (P2P) is a PCIe feature which enables
two PCIe devices to directly transfer data between each other without
using host RAM as a temporary storage. The latest version of Vitis PCIe
platforms support P2P feature via PCIe Resizeable BAR Capability

In this Example we highlight the data transfer between SSD and FPGA 
and capture the performance numbers for p2p transfer where the input
path(/dev/nvme0n1) is provided using -p argument.

Following is the real log 
reported while running the design on U2 platform:

::

   root@hem-PowerEdge-T640:~# ./p2p_bandwidth -x ./bandwidth.xclbin -p /dev/nvme0n1 
   Found Platform
   Platform Name: Xilinx
   INFO: Reading ./bandwidth.xclbin
   Loading: './bandwidth.xclbin'
   Trying to program device[0]: xilinx_u2_gen3x4_xdma_gc_base_1
   Device[0]: program successful!
   ############################################################
                     Writing data to SSD                       
   ############################################################
   INFO: Successfully opened NVME SSD /dev/nvme0n1

   Map P2P device buffers to host access pointers

   Start P2P Write of various buffer sizes from device buffers to SSD

   Buffer = 4.00 KB Iterations = 262144 Throughput = 0.16GB/s
   Buffer = 8.00 KB Iterations = 131072 Throughput = 0.25GB/s
   Buffer = 16.00 KB Iterations = 65536 Throughput = 0.37GB/s
   Buffer = 32.00 KB Iterations = 32768 Throughput = 0.50GB/s
   Buffer = 64.00 KB Iterations = 16384 Throughput = 0.68GB/s
   Buffer = 128.00 KB Iterations = 8192 Throughput = 0.82GB/s
   Buffer = 256.00 KB Iterations = 4096 Throughput = 0.92GB/s
   Buffer = 512.00 KB Iterations = 2048 Throughput = 0.96GB/s
   Buffer = 1.00 MB Iterations = 1024 Throughput = 0.99GB/s
   Buffer = 2.00 MB Iterations = 512 Throughput = 1.00GB/s
   Buffer = 4.00 MB Iterations = 256 Throughput = 1.02GB/s
   Buffer = 8.00 MB Iterations = 128 Throughput = 1.03GB/s
   Buffer = 16.00 MB Iterations = 64 Throughput = 1.03GB/s
   Buffer = 32.00 MB Iterations = 32 Throughput = 1.04GB/s
   Buffer = 64.00 MB Iterations = 16 Throughput = 1.04GB/s
   Buffer = 128.00 MB Iterations = 8 Throughput = 1.04GB/s
   ############################################################
                     Reading data from SSD                       
   ############################################################
   INFO: Successfully opened NVME SSD /dev/nvme0n1
   
   Map P2P device buffers to host access pointers
   
   Start P2P Read of various buffer sizes from SSD to device buffers
   
   Buffer = 4.00 KB Iterations = 262144 Throughput = 0.05GB/s
   Buffer = 8.00 KB Iterations = 131072 Throughput = 0.17GB/s
   Buffer = 16.00 KB Iterations = 65536 Throughput = 0.31GB/s
   Buffer = 32.00 KB Iterations = 32768 Throughput = 0.54GB/s
   Buffer = 64.00 KB Iterations = 16384 Throughput = 0.87GB/s
   Buffer = 128.00 KB Iterations = 8192 Throughput = 1.21GB/s
   Buffer = 256.00 KB Iterations = 4096 Throughput = 1.55GB/s
   Buffer = 512.00 KB Iterations = 2048 Throughput = 1.37GB/s
   Buffer = 1.00 MB Iterations = 1024 Throughput = 1.59GB/s
   Buffer = 2.00 MB Iterations = 512 Throughput = 1.99GB/s
   Buffer = 4.00 MB Iterations = 256 Throughput = 2.15GB/s
   Buffer = 8.00 MB Iterations = 128 Throughput = 2.26GB/s
   Buffer = 16.00 MB Iterations = 64 Throughput = 2.28GB/s
   Buffer = 32.00 MB Iterations = 32 Throughput = 2.33GB/s
   Buffer = 64.00 MB Iterations = 16 Throughput = 2.35GB/s
   Buffer = 128.00 MB Iterations = 8 Throughput = 2.36GB/s
   TEST PASSED

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.