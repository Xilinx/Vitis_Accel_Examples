Loop Dependency Inter (C)
======================

This Example demonstrates the HLS pragma 'DEPENDENCE'.Using 'DEPENDENCE' pragma, user can provide additional dependency details to the compiler by specifying if the dependency in consecutive loop iterations on buffer is true/false, which allows the compiler to perform unrolling/pipelining to get better performance.

***KEY CONCEPTS:*** Inter Dependence

***KEYWORDS:*** DEPENDENCE, inter, WAR

##  DESIGN FILES
Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

```
src/host.cpp
src/vconv.cpp
src/vconv.h
```

##  COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./host <vconv XCLBIN>
```

