Stream Vector Addition with OpenCL Buffers
======================

This is a simple streaming Vector Addition C Kernel design with 1 Stream input, 1 memory mapped input to the kernel, and 1 stream output that demonstrates on how to process a stream of data for computation along with OpenCL buffers.

***KEY CONCEPTS:*** Read/Write Stream, Create/Release Stream

***KEYWORDS:*** cl_stream, CL_STREAM_EOT, CL_STREAM_BLOCKING

## SUPPORTED PLATFORMS
Platform | Board             | Software Version
---------|-------------------|-----------------
xilinx_u200_qdma|Xilinx Alveo U200|SDx 2019.1
xilinx_u250_qdma|Xilinx Alveo U250|SDx 2019.1


##  DESIGN FILES
Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

```
src/host.cpp
src/krnl_stream_vadd.cpp
```

##  COMMAND LINE ARGUMENTS
Once the environment has been configured, the application can be executed by
```
./vadd_stream <krnl_stream_vadd XCLBIN>
```

