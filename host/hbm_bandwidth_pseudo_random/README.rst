HBM Bandwidth - Pseudo Random Ethash
====================================

This is a HBM bandwidth example using a pseudo random 1024 bit data access pattern to mimic Ethereum Ethash workloads. The design contains 3 compute units of a kernel, reading 1024 bits from a pseudo random address in each of 2 pseudo channels and writing the results of a simple mathematical operation to a pseudo random address in 2 other pseudo channels. To maximize bandwidth the pseudo channels are used in  P2P like configuration - See https://developer.xilinx.com/en/articles/maximizing-memory-bandwidth-with-vitis-and-xilinx-ultrascale-hbm-devices.html for more information on HBM memory access configurations. The host application allocates buffers in 12  HBM banks and runs the compute units concurrently to measure the overall bandwidth between kernel and HBM Memory.

**KEY CONCEPTS:** High Bandwidth Memory, Multiple HBM Banks, Random Memory Access, Linear Feedback Shift Register

**KEYWORDS:** HBM, XCL_MEM_TOPOLOGY, cl_mem_ext_ptr_t

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   _u25_
   u200
   zc
   vck
   u250
   aws
   samsung

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

   ./hbm_bandwidth_pseudo_random <krnl_vaddmul XCLBIN>

