HBM Simple Vector Addition
===========================
This example demonstrates how HBM(High Bandwidth Memory) can be interfaced to reduce the latency of global memory transfers.
Two cases are executed, one HBM bank used for two inputs and one output; three banks used one each for two inputs and one output. 
Individual bank for each buffer will provide more bandwidth for the application.

HBM is a high performance RAM interface for 3D-stacked DRAM. HBM can provide bandwidth upto 307 GB/s with low power consumption
(20% as compared to GDDR5). There are 32 memory resources referenced as HBM[0:31] by XOCC and these are accessed via 16 memory
controllers.  

cl_mem_ext_ptr needs to be used in cases where memory assingment is done by user.

```c++
cl_mem_ext_ptr_t inBufExt1, inBufExt2, outBufExt;
inBufExt1.obj = source_in1.data();
inBufExt1.param = 0;
inBufExt1.flags = bank_assign[0];
```
`CL_MEM_EXT_PTR_XILINX` must be specified while creating buffers using HBM.
```c++
cl::Buffer buffer_input1(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                          sizeof(uint32_t) * size, &inBufExt1, &err));
```
HBM memory must be associated to respective kernel I/O ports using `--sp` option in kernel linking stage.
  
`LDCLFLAGS += --sp krnl_vadd_1.m_axi_gmem0:HBM[0:3] --sp krnl_vadd_1.m_axi_gmem1:HBM[0:3]
`
