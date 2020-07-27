HBM Simple
==========

This is Simple example to demonstrate how to use HBM Platform. Two cases
are executed, Single HBM bank is used for two inputs and one output;
three separate banks are used for each argument of kernels. Individual
bank for each buffer will provide more bandwidth for the application.

``cl_mem_ext_ptr`` object needs to be used in cases where memory
assignment is done by user.

.. code:: cpp

   cl_mem_ext_ptr_t bufExt;
   bufExt.obj = source_in1.data();
   bufExt.param = 0;
   bufExt.flags = n | XCL_MEM_TOPOLOGY; // specify n the bank number which needs to be targeted (0:31)
   cl::Buffer buffer_input1(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                             sizeof(uint32_t) * size, &bufExt, &err));

compute unit interfaces are associated to fixed HBM banks using
``krnl_vadd.ini`` file:

::

   [connectivity]
   sp=krnl_vadd_1.in1:HBM[0:3]
   sp=krnl_vadd_1.in2:HBM[0:3]
   sp=krnl_vadd_1.out_r:HBM[0:3]

It specify that kernel interfaces (in1,in2 and out_r) should be
connected such a way that it should have access to HBM banks 0 to 3.
System linker will make sure this requirement while building the design.

For Case1, all three buffers (in1,in2, and out_r) will be created inside
Single bank and application will run and performance will be reported.
For Case2, All three buffers will be created into different banks and
application will run and performance will be reported. For Case2, three
different banks will give higher DDR bandwidth compare to case1, so
performance of Case2 will be better compare to case1. Following is the
real log reported while running on U280 platform:

::

   Platform Name: Xilinx
   Found Device=xilinx_u280_xdma_201920_1
   INFO: Reading ./build_dir.hw.xilinx_u280_xdma_201920_1/krnl_vadd.xclbin
   Loading: './build_dir.hw.xilinx_u280_xdma_201920_1/krnl_vadd.xclbin'
   Running CASE 1  : Single HBM for all three Buffers 
   Picking Buffer size 67108864 so that all three buffer should fit into Single HBM (max 256MB)
   Each buffer is allocated with same HBM bank.
   input 1 -> bank 0 
   input 2 -> bank 0 
   output  -> bank 0 
   [CASE 1] THROUGHPUT = 9.54234 GB/s
   Running CASE 2: Three Separate Banks for Three Buffers
   For This case each buffer will be having different HBM, so buffer size is picked to utilize full HBM
   vector size is 268435456 as maximum possible inside single HBM
   Each buffer is allocated with different HBM bank.
   input 1 -> bank 1 
   input 2 -> bank 2 
   output  -> bank 3 
   [CASE 2] THROUGHPUT = 38.7406 GB/s 
   TEST PASSED
