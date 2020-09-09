BIND OP and STORAGE (HLS C/C++ Kernel)
======================================

This is simple example of vector addition to describe how to use BIND OP and STORAGE for better implementation style.

**KEY CONCEPTS:** BIND OP, BIND STORAGE

**KEYWORDS:** BIND_OP, BIND_STORAGE, impl, op, type, latency

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   zcu102
   zc706

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

   ./bind_op_storage <vadd XCLBIN>

