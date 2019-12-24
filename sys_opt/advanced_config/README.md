Advanced Configuration
======================

This is simple example to describe how user can provide various advanced options in Vitis flow.

***KEY CONCEPTS:*** Using custom Tcl, Setting Kernel Clock Frequency, Clock Uncertainity

***KEYWORDS:*** config, set_clock_uncertainity, kernel_frequency, hls_pre_tcl

##  DESIGN FILES
Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

```
src/host.cpp
src/vadd.cpp
```

##  COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./host <vadd XCLBIN>
```

