Dataflow Function OpenCL(CL)
======================

This is simple example of vector addition to demonstrate Dataflow functionality in OpenCL Kernel. OpenCL Dataflow allows user to run multiple functions together to achieve higher throughput.

***KEY CONCEPTS:*** Function/Task Level Parallelism

***KEYWORDS:*** xcl_dataflow, xclDataflowFifoDepth

##  DESIGN FILES
Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

```
src/adder.cl
src/host.cpp
```

##  COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./host <adder XCLBIN>
```

