Burst Read/Write (C)
======================

This is simple example of using AXI4-master interface for burst read and write

***KEY CONCEPTS:*** burst access

***KEYWORDS:*** memcpy, max_read_burst_length, max_write_burst_length

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

