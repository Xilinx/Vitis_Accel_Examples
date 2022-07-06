P2P FPGA2FPGA XRT (XRT Native API's)
====================================

This example illustrates the date transfer between two FPGA
using p2p without migrating the data via host memory space. In this example 
we create p2p buffer using ``p2p`` flag.

.. code:: cpp

   xrt::bo::flags flags = xrt::bo::flags::p2p;
   auto out1 = xrt::bo(device1, vector_size_bytes, flags, krnl.group_id(1));

Following is the real log reported while running on U50 platform and U200 platform:

::

   Open the device1 :0 
   Load the xclbin ./increment.xclbin
   Open the device2 : 1
   Allocate Buffer in Global Memory
   synchronize input buffer data to device global memory
   Execution of the kernel
   Get the output data from the device
   Total Time : 208670 (microseconds)
   TEST PASSED
