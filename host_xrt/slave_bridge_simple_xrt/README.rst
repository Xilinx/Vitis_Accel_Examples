Slave Bridge Simple XRT (XRT Native API's)
==========================================

This is simple slave bridge example to describe how a user kernel can access the host memory using xrt native api's.

**KEY CONCEPTS:** slave bridge, address translation unit

**KEYWORDS:** host_only, HOST[0]

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

   src/host.cpp
   src/kernel.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./slave_bridge_simple_xrt -x <krnl_vadd XCLBIN>

