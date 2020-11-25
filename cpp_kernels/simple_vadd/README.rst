Vector Addition
===============

This is a simple example of vector addition.The purpose of this code is to introduce the user to application development in the Vitis tools.

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   nodma

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/krnl_vadd.cpp
   src/vadd.cpp
   src/vadd.h
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./simple_vadd <krnl_vadd XCLBIN>

