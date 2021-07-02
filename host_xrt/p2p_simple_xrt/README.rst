P2P Simple Example XRT (XRT Native API's)
=========================================

This is simple example of vector increment to describe P2P between FPGA and NVMe SSD using xrt native api's.

**KEY CONCEPTS:** `P2P <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/Chunk938767849.html#qex1558551641915>`__, NVMe SSD, SmartSSD

**KEYWORDS:** xrt::bo::flags::p2p, pread, pwrite, O_DIRECT, O_RDWR

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   zc
   _u25_
   u30
   vck
   aws
   201910
   u50_gen3x16_xdma_2019

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   data/sample.txt
   src/adder.cpp
   src/host.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./p2p_simple_xrt -x <adder XCLBIN> -f ./data/sample.txt

DETAILS
-------

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
    

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.