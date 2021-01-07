Hello World XRT (XRT Native API's)
==================================

This is simple example of vector addition to describe the usage of XRT Native API's.

**KEY CONCEPTS:** XRT Native APIs

**KEYWORDS:** xrt::device, load_xclbin, xrt::bo, xrt::kernel, map, sync, XCL_BO_SYNC_BO_TO_DEVICE, XCL_BO_SYNC_BO_FROM_DEVICE

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/vadd.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./hello_world_xrt -x <vadd XCLBIN>

