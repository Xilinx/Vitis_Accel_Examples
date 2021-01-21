HBM Simple XRT (XRT Native API's)
=================================

This is a simple example of vector addition to describe how to use HLS kernels with HBM (High Bandwidth Memory) for achieving high throughput using xrt native api's.

**KEY CONCEPTS:** High Bandwidth Memory, Multiple HBM Banks

**KEYWORDS:** HBM

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
   aws
   samsung
   nodma

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/krnl_vadd.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./hbm_simple_xrt  -x <krnl_vadd XCLBIN>

