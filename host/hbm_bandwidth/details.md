HBM Bandwidth Test
===================

This is host application to test HBM interface bandwidth. Design contains 8 compute units of Kernel. Each compute unit has full access to all HBM memory (0 to 31). Host application allocate buffers into all 32 HBM Banks (16 Input buffers and 16 output buffers). Host application runs all 8 compute units together and measures the overall HBM bandwidth.

HBM is a high performance RAM interface for 3D-stacked DRAM. HBM can provide very  high bandwidth  greater than **400 GB/s** with low power consumption (20% as compared to GDDR5). These 32 memory resources referenced as HBM [0:31] by v++ and these are accessed via 16 memory controllers.

Host can allocate a buffer into specific HBM bank using `CL_MEM_EXT_PTR_XILINX` flag of buffer. `cl_mem_ext_ptr` object needs to be used in cases where memory assignment is done by user explicitly:

```c++
cl_mem_ext_ptr_t bufExt;
bufExt.obj = host_pointer;
bufExt.param = 0;
bufExt.flags = n  | XCL_MEM_TOPOLOGY; 
buffer_input = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR, size, &bufExt, &err));
```

HBM memory must be associated to respective kernel I/O ports using `--sp` option in kernel linking stage.

```
LDCLFLAGS += --sp krnl_vaddmul_1.m_axi_gmem0:HBM[0] --sp krnl_vaddmul_1.m_axi_gmem1:HBM[1] 
             --sp krnl_vaddmul_1.m_axi_gmem2:HBM[2] --sp krnl_vaddmul_1.m_axi_gmem3:HBM[3]
```
