HBM Bandwidth for large buffers using RAMA IP
=============================================

This is host application to test HBM interface bandwidth for buffers > 256 MB with pseudo random 1024 bit data access pattern, mimicking Ethereum Ethash workloads. Design contains 4 compute units of Kernel, 2 with and 2 without RAMA IP. Each compute unit reads 1024 bits from a pseudo random address in each of 2 pseudo channel groups and writes the results of a simple mathematical operation to a pseudo random address in 2 other pseudo channel groups. Each buffer is 1 GB large requiring 4 HBM banks. Since the first 2 CUs requires 4 buffers each and are then used again by the other 2 CUs, the .cfg file is allocating the buffers to all the 32 HBM banks.  The host application runs the compute units concurrently to measure the overall bandwidth between kernel and HBM Memory.

**KEY CONCEPTS:** High Bandwidth Memory, Multiple HBM Banks, Random Memory Access, Linear Feedback Shift Register, RAMA IP

**KEYWORDS:** HBM, ra_master_interface

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   _u25_
   u30
   u200
   zc
   vck
   u250
   aws-vu9p-f1
   _u2_
   nodma

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/krnl_vaddmul.cpp
   src/krnl_vaddmul.h
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./hbm_rama_ip <krnl_vaddmul XCLBIN>

