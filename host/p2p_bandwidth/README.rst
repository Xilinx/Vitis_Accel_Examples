P2P bandwidth Example
=====================

This is simple example to test data transfer between SSD and FPGA.

**KEY CONCEPTS:** P2P, SmartSSD, XDMA

**KEYWORDS:** XCL_MEM_EXT_P2P_BUFFER, pread, pwrite

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   zc
   vck
   aws-vu9p-f1
   2018
   201910
   _u25_
   u30

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/bandwidth.cpp
   src/host.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./p2p_bandwidth -x <bandwidth XCLBIN>

DETAILS
-------

PCIe peer-to-peer communication (P2P) is a PCIe feature which enables
two PCIe devices to directly transfer data between each other without
using host RAM as a temporary storage. The latest version of Vitis PCIe
platforms support P2P feature via PCIe Resizeable BAR Capability

In this Example we highlight the data transfer between SSD and FPGA 
and capture the performance numbers for p2p transfer.

Following is the real log 
reported while running the design on U50 platform:

::

   Found Platform
   Platform Name: Xilinx
   INFO: Reading /opt/xilinx/firmware/u50/gen3x16-xdma/base/test/bandwidth.xclbin
   Loading: '/opt/xilinx/firmware/u50/gen3x16-xdma/base/test/bandwidth.xclbin'
   Trying to program device[0]: xilinx_u50_gen3x16_xdma_base_4
   Device[0]: program successful!
   ############################################################
                  Writing data to SSD                       
   ############################################################
   INFO: Successfully opened NVME SSD /dev/nvme0n1
   
   Map P2P device buffers to host access pointers
   
   Now start P2P Write from device buffers to SSD
   
   Now start P2P copy 1048576 Bytes from a device to another device
   {"metric": "copy_latency", "buf_size_bytes": 1048576, "number_of_bufs": 128, "per_buffer_latency_sec": 0.000466641}
   {"metric": "copy_throughput", "buf_size_bytes": 1048576, "number_of_bufs": 128, "throughput_gb_per_sec": 2.09275}
   Now start P2P copy 2097152 Bytes from a device to another device
   {"metric": "copy_latency", "buf_size_bytes": 2097152, "number_of_bufs": 64, "per_buffer_latency_sec": 0.000748203}
   {"metric": "copy_throughput", "buf_size_bytes": 2097152, "number_of_bufs": 64, "throughput_gb_per_sec": 2.61042}
   Now start P2P copy 4194304 Bytes from a device to another device
   {"metric": "copy_latency", "buf_size_bytes": 4194304, "number_of_bufs": 32, "per_buffer_latency_sec": 0.00160703}
   {"metric": "copy_throughput", "buf_size_bytes": 4194304, "number_of_bufs": 32, "throughput_gb_per_sec": 2.43072}
   Now start P2P copy 8388608 Bytes from a device to another device
   {"metric": "copy_latency", "buf_size_bytes": 8388608, "number_of_bufs": 16, "per_buffer_latency_sec": 0.00353706}
   {"metric": "copy_throughput", "buf_size_bytes": 8388608, "number_of_bufs": 16, "throughput_gb_per_sec": 2.20875}
   Now start P2P copy 16777216 Bytes from a device to another device
   {"metric": "copy_latency", "buf_size_bytes": 16777216, "number_of_bufs": 8, "per_buffer_latency_sec": 0.00659838}
   {"metric": "copy_throughput", "buf_size_bytes": 16777216, "number_of_bufs": 8, "throughput_gb_per_sec": 2.36801}
   Now start P2P copy 33554432 Bytes from a device to another device
   {"metric": "copy_latency", "buf_size_bytes": 33554432, "number_of_bufs": 4, "per_buffer_latency_sec": 0.0145725}
   {"metric": "copy_throughput", "buf_size_bytes": 33554432, "number_of_bufs": 4, "throughput_gb_per_sec": 2.14445}
   Now start P2P copy 67108864 Bytes from a device to another device
   {"metric": "copy_latency", "buf_size_bytes": 67108864, "number_of_bufs": 2, "per_buffer_latency_sec": 0.0264645}
   {"metric": "copy_throughput", "buf_size_bytes": 67108864, "number_of_bufs": 2, "throughput_gb_per_sec": 2.36165}
   Now start P2P copy 134217728 Bytes from a device to another device
   {"metric": "copy_latency", "buf_size_bytes": 134217728, "number_of_bufs": 1, "per_buffer_latency_sec": 0.058277}
   {"metric": "copy_throughput", "buf_size_bytes": 134217728, "number_of_bufs": 1, "throughput_gb_per_sec": 2.14493}

   ############################################################
                  Reading data from SSD                       
   ############################################################
   INFO: Successfully opened NVME SSD /dev/nvme0n1
   
   Map P2P device buffers to host access pointers
   
   Now start P2P Read from SSD to device buffers
   
   Now start P2P copy 1048576 Bytes from a device to another device
   {"metric": "copy_latency", "buf_size_bytes": 1048576, "number_of_bufs": 128, "per_buffer_latency_sec": 0.000622}
   {"metric": "copy_throughput", "buf_size_bytes": 1048576, "number_of_bufs": 128, "throughput_gb_per_sec": 1.57004}
   Now start P2P copy 2097152 Bytes from a device to another device
   {"metric": "copy_latency", "buf_size_bytes": 2097152, "number_of_bufs": 64, "per_buffer_latency_sec": 0.000747469}
   {"metric": "copy_throughput", "buf_size_bytes": 2097152, "number_of_bufs": 64, "throughput_gb_per_sec": 2.61299}
   Now start P2P copy 4194304 Bytes from a device to another device
   {"metric": "copy_latency", "buf_size_bytes": 4194304, "number_of_bufs": 32, "per_buffer_latency_sec": 0.00137222}
   {"metric": "copy_throughput", "buf_size_bytes": 4194304, "number_of_bufs": 32, "throughput_gb_per_sec": 2.84667}
   Now start P2P copy 8388608 Bytes from a device to another device
   {"metric": "copy_latency", "buf_size_bytes": 8388608, "number_of_bufs": 16, "per_buffer_latency_sec": 0.00263306}
   {"metric": "copy_throughput", "buf_size_bytes": 8388608, "number_of_bufs": 16, "throughput_gb_per_sec": 2.96708}
   Now start P2P copy 16777216 Bytes from a device to another device
   {"metric": "copy_latency", "buf_size_bytes": 16777216, "number_of_bufs": 8, "per_buffer_latency_sec": 0.00512488}
   {"metric": "copy_throughput", "buf_size_bytes": 16777216, "number_of_bufs": 8, "throughput_gb_per_sec": 3.04885}
   Now start P2P copy 33554432 Bytes from a device to another device
   {"metric": "copy_latency", "buf_size_bytes": 33554432, "number_of_bufs": 4, "per_buffer_latency_sec": 0.0102465}
   {"metric": "copy_throughput", "buf_size_bytes": 33554432, "number_of_bufs": 4, "throughput_gb_per_sec": 3.04982}
   Now start P2P copy 67108864 Bytes from a device to another device
   {"metric": "copy_latency", "buf_size_bytes": 67108864, "number_of_bufs": 2, "per_buffer_latency_sec": 0.0206}
   {"metric": "copy_throughput", "buf_size_bytes": 67108864, "number_of_bufs": 2, "throughput_gb_per_sec": 3.03398}
   Now start P2P copy 134217728 Bytes from a device to another device
   {"metric": "copy_latency", "buf_size_bytes": 134217728, "number_of_bufs": 1, "per_buffer_latency_sec": 0.040157}
   {"metric": "copy_throughput", "buf_size_bytes": 134217728, "number_of_bufs": 1, "throughput_gb_per_sec": 3.11278}

    TEST PASSED


For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.