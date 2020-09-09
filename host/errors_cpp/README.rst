Error Handling (CPP)
====================

This example discuss the different reasons for errors in OpenCL C++ and how to handle them at runtime.

**KEY CONCEPTS:** OpenCL C++ API, Error handling

**KEYWORDS:** CL_SUCCESS, CL_DEVICE_NOT_FOUND, CL_DEVICE_NOT_AVAILABLE, CL_INVALID_VALUE, CL_INVALID_KERNEL_NAME, CL_INVALID_BUFFER_SIZE

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

   ./errors_cpp <vector_addition XCLBIN>

