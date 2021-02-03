P2P Simple Example XRT (XRT Native API's)
=========================================

This is simple example of vector increment to describe P2P between FPGA and NVMe SSD using xrt native api's.

**KEY CONCEPTS:** P2P, NVMe SSD, SmartSSD

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
   2018
   201910

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/adder.cpp
   src/host.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./p2p_simple_xrt -x <adder XCLBIN>

