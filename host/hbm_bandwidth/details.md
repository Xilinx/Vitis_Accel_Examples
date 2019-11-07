HBM Bandwidth Test
===================

This is host application to test HBM interface bandwidth. Design contains 3 compute units of Kernel. Each compute unit has full access to all HBM memory (0 to 31). Host application allocate buffers into all 32 HBM Banks (16 Input buffers and 16 output buffers). Host application runs all 3 compute units together and measures the overall HBM bandwidth.

HBM is a high performance RAM interface for 3D-stacked DRAM. HBM can provide very  high bandwidth  greater than **400 GB/s** with low power consumption (20% as compared to GDDR5). These 32 memory resources referenced as HBM [0:31] by v++ and these are accessed via 16 memory controllers.

Host can allocate a buffer into specific HBM bank using `CL_MEM_EXT_PTR_XILINX` flag of buffer. `cl_mem_ext_ptr` object needs to be used in cases where memory assignment is done by user explicitly:

```c++
cl_mem_ext_ptr_t bufExt;
bufExt.obj = host_pointer;
bufExt.param = 0;
bufExt.flags = n  | XCL_MEM_TOPOLOGY; 
buffer_input = cl::Buffer(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR, size, &bufExt, &err));
```

HBM memory must be associated to respective kernel I/O ports using `sp` option. We need to add mapping between HBM memory and I/O ports in krnl_vaddmul.ini file

```
sp=krnl_vaddmul_1.in1:HBM[0]
sp=krnl_vaddmul_1.in2:HBM[1] 
sp=krnl_vaddmul_1.out_add:HBM[2]
sp=krnl_vaddmul_1.out_mul:HBM[3]
```

To see the benifit of HBM, user can look into the runtime logs and see the overall throughput.
```
Loading: './build_dir.hw.xilinx_u50_xdma_201920_1/krnl_vaddmul.xclbin'
Creating a kernel [krnl_vaddmul:{krnl_vaddmul_1}] for CU(1)
Creating a kernel [krnl_vaddmul:{krnl_vaddmul_2}] for CU(2)
Creating a kernel [krnl_vaddmul:{krnl_vaddmul_3}] for CU(3)
THROUGHPUT = 158.3 GB/s
TEST PASSED
```
By default we are going with 3 compute units of kernel as we have power consumption limitation while targeting U50 platform. We have the design support for 8 compute units of Kernel. To run the HBM Bandwidth with 8 compute units targeting U280 platform, user need to update krnl_vaddmul.ini and host.cpp files.
Add the following lines in krnl_vaddmul.ini file
```
sp=krnl_vaddmul_4.in1:HBM[12]
sp=krnl_vaddmul_4.in2:HBM[13]
sp=krnl_vaddmul_4.out_add:HBM[14]
sp=krnl_vaddmul_4.out_mul:HBM[15]
sp=krnl_vaddmul_5.in1:HBM[16]
sp=krnl_vaddmul_5.in2:HBM[17]
sp=krnl_vaddmul_5.out_add:HBM[18]
sp=krnl_vaddmul_5.out_mul:HBM[19]
sp=krnl_vaddmul_6.in1:HBM[20]
sp=krnl_vaddmul_6.in2:HBM[21]
sp=krnl_vaddmul_6.out_add:HBM[22]
sp=krnl_vaddmul_6.out_mul:HBM[23]
sp=krnl_vaddmul_7.in1:HBM[24]
sp=krnl_vaddmul_7.in2:HBM[25] 
sp=krnl_vaddmul_7.out_add:HBM[26]
sp=krnl_vaddmul_7.out_mul:HBM[27]
sp=krnl_vaddmul_8.in1:HBM[28]
sp=krnl_vaddmul_8.in2:HBM[29] 
sp=krnl_vaddmul_8.out_add:HBM[30]
sp=krnl_vaddmul_8.out_mul:HBM[31]
nk=krnl_vaddmul:8
```
In host.cpp file user need to change the #define NUM_KERNEL from 3 to 8
```
#define NUM_KERNEL 8
```
User can see higher benifit of HBM when ran on U280 platform with 8 compute units of kernel. Below is the runtime log which shows overall throughput greater than 400GBps
```
Loading: './build_dir.hw.xilinx_u280_xdma_201920_1/krnl_vaddmul.xclbin'
Creating a kernel [krnl_vaddmul:{krnl_vaddmul_1}] for CU(1)
Creating a kernel [krnl_vaddmul:{krnl_vaddmul_2}] for CU(2)
Creating a kernel [krnl_vaddmul:{krnl_vaddmul_3}] for CU(3)
Creating a kernel [krnl_vaddmul:{krnl_vaddmul_4}] for CU(4)
Creating a kernel [krnl_vaddmul:{krnl_vaddmul_5}] for CU(5)
Creating a kernel [krnl_vaddmul:{krnl_vaddmul_6}] for CU(6)
Creating a kernel [krnl_vaddmul:{krnl_vaddmul_7}] for CU(7)
Creating a kernel [krnl_vaddmul:{krnl_vaddmul_8}] for CU(8)
THROUGHPUT = 421.3 GB/s
TEST PASSED
```

