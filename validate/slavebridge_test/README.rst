Slave Bridge (C)
================

This slave bridge bandwidth design explains how direct host memory access can be done by the kernel.

**KEY CONCEPTS:** slave bridge, address translation unit

**KEYWORDS:** XCL_MEM_EXT_HOST_ONLY, HOST[0]

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   zc
   vck
   aws-vu9p-f1
   2019
   2018
   202010
   _u2_
   _u25_
   u30

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/slavebridge.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./slavebridge.exe platform_test_path

