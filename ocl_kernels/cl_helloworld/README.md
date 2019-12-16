Hello World (CL)
======================

This example is a simple OpenCL application. It will highlight the basic flow of an OpenCL application.

***KEY CONCEPTS:*** OpenCL API

##  DESIGN FILES
Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

```
src/host.cpp
src/vector_addition.cl
```

##  COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./helloworld <vector_addition XCLBIN>
```

