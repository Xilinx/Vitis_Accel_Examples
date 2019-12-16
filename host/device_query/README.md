Device Query (CPP)
======================

This Example prints the OpenCL properties of the platform and its devices using OpenCL CPP APIs. It also displays the limits and capabilities of the hardware.

***KEY CONCEPTS:*** OpenCL API, Querying device properties

## EXCLUDED PLATFORMS
Platforms containing following strings in their names are not supported for this example :
```
zc
```

##  DESIGN FILES
Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

```
src/host.cpp
```

##  COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./device_query
```

