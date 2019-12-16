Multiple Devices (C)
======================

This example show how to take advantage of multiple FPGAs on a system. It will show how to initialized an OpenCL context, allocate memory on the two devices and execute a kernel on each FPGA.

***KEY CONCEPTS:*** OpenCL API, Multi-FPGA Execution, Event Handling

***KEYWORDS:*** xcl::get_xil_devices

##  DESIGN FILES
Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

```
src/host.cpp
src/vector_addition.cpp
```

##  COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./multiple_devices <vector_addition XCLBIN>
```

