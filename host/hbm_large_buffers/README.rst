HBM Large Buffers
=================

This is a simple example of vector addition to describe how HBM banks can be grouped to handle buffers larger than 256 MB.

**KEY CONCEPTS:** High Bandwidth Memory, Multiple HBM Bank Groups

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
   u280
   aws-vu9p-f1
   _u2_
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

   ./hbm_large_buffers <krnl_vadd XCLBIN>

