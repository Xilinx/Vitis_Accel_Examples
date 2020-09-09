Error Handling (C)
==================

This example discuss the different reasons for errors in OpenCL and how to handle them at runtime.

**KEY CONCEPTS:** OpenCL API, Error handling

**KEYWORDS:** CL_SUCCESS, CL_DEVICE_NOT_FOUND, CL_DEVICE_NOT_AVAILABLE

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/vector_addition.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./errors <vector_addition XCLBIN>

