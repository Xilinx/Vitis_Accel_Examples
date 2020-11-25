Slave Bridge Bandwidth(C)
=========================

This is slave bridge bandwidth example to describe host memory and kernel bandwidth test.

**KEY CONCEPTS:** slave bridge, bandwidth, address translation unit

**KEYWORDS:** XCL_MEM_EXT_HOST_ONLY, HOST[0]

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   _u25_
   u30
   zc
   vck
   aws
   2019
   2018
   202010
   samsung

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/bandwidth.cpp
   src/host.cpp
   src/read_bandwidth.cpp
   src/write_bandwidth.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./slave_bridge_bw.exe <bandwidth XCLBIN>

