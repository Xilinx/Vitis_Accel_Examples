Kernel Global Bandwidth
=======================

Bandwidth test of global to local memory.

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   u50
   vck
   zc

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/kernel.cpp
   src/kernel_global_bandwidth.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./kernel_global_bandwidth <krnl_kernel_global XCLBIN>

