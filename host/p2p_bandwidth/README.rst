P2P bandwidth Example
=====================

This is simple example to test Synchronous and Asyncronous data transfer between SSD and FPGA.

**KEY CONCEPTS:** P2P, SmartSSD, XDMA

**KEYWORDS:** XCL_MEM_EXT_P2P_BUFFER, pread, pwrite

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   zc
   vck
   _u25_
   u200
   u250
   u50
   u280
   aws

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/copy_kernel.cpp
   src/host.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./p2p_bandwidth -x <copy_kernel XCLBIN> -i /dev/nvme0n1

