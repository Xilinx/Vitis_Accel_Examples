Overlap Host and Kernel (C)
======================

This examples demonstrates techniques that allow user to overlap Host(CPU) and FPGA computation in an application. It will cover asynchronous operations and event object.

***KEY CONCEPTS:*** OpenCL API, Synchronize Host and FPGA, Asynchronous Processing, Events, Asynchronous memcpy

***KEYWORDS:*** cl_event, cl::CommandQueue, CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, enqueueMigrateMemObjects

##  DESIGN FILES
Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

```
src/host.cpp
src/vector_addition.cpp
```

##  COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./overlap <vector_addition XCLBIN>
```

