Wide Memory Read/Write (CL)
======================

This is simple example of vector addition to demonstrate Wide Memory Access using uint16 data type. Based on input argument type, xocc compiler will figure our the memory datawidth between Global Memory and Kernel. For this example, uint16 datatype is used, so Memory datawidth will be 16 x (integer bit size) = 16 x 32 = 512 bit.

***KEY CONCEPTS:*** Kernel to DDR, wide memory access, burst read and write

***KEYWORDS:*** uint16, xcl_pipeline_loop


For Setup instructions, please refer: [setup.md][]

[setup.md]: setup.md

