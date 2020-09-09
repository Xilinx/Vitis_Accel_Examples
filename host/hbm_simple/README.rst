HBM Simple
==========

This is a simple example of vector addition to describe how to use HLS kernels with HBM (High Bandwidth Memory) for achieving high throughput.

**KEY CONCEPTS:** High Bandwidth Memory, Multiple HBM Banks

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
   src/krnl_vadd.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./hbm_simple <krnl_vadd XCLBIN>

