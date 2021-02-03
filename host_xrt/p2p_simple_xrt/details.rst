P2P Simple XRT (XRT Native API's)
=================================

This example illustrates the date transfer between FPGA device and NVMe device
using p2p without migrating the data via host memory space. In this example 
we create p2p buffer using ``p2p`` flag.

.. code:: cpp

   xrt::bo::flags flags = xrt::bo::flags::p2p;
   auto p2p_bo0 = xrt::bo(device, vector_size_bytes, flags, krnl.group_id(0));

Following is the real log reported while running on U50 platform and NVMe:

::

   Open the device0
   Load the xclbin ./adder.xclbin
   ############################################################
                     Writing data to SSD                       
   ############################################################
   INFO: Successfully opened NVME SSD /dev/nvme0n1
   synchronize input buffer data to device global memory
   Execution of the kernel
   Now start P2P Write from device buffers to SSD
   
   ############################################################
                     Reading data from SSD                       
   ############################################################
   INFO: Successfully opened NVME SSD /dev/nvme0n1
   Allocate Buffer in Global Memory
   Now start P2P Read from SSD to device buffers
   
   Execution of the kernel
   Get the output data from the device
   TEST PASSED
    
