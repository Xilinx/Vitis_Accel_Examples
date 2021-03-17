HBM Simple
==========

This is Simple example to demonstrate how to use HBM Platform. Two cases
are executed, Single HBM Pseudo-Channel(PC) is used for two inputs and one output;
three separate PCs are used for each argument of kernels. Individual
PC for each buffer will provide more bandwidth for the application.

``cl_mem_ext_ptr`` object needs to be used in cases where memory
assignment is done by user.

.. code:: cpp

   cl_mem_ext_ptr_t bufExt;
   bufExt.obj = source_in1.data();
   bufExt.param = 0;
   bufExt.flags = n | XCL_MEM_TOPOLOGY; // specify n the PC number which needs to be targeted (0:31)
   cl::Buffer buffer_input1(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR,
                             sizeof(uint32_t) * size, &bufExt, &err));

compute unit interfaces are associated to fixed HBM PCs using
``krnl_vadd.cfg`` file:

::

   [connectivity]
   sp=krnl_vadd_1.in1:HBM[0:3]
   sp=krnl_vadd_1.in2:HBM[0:3]
   sp=krnl_vadd_1.out_r:HBM[0:3]

It specify that kernel interfaces (in1,in2 and out_r) should be
connected such a way that it should have access to HBM PCs 0 to 3.
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
   INFO: Reading ./build_dir.hw.xilinx_u50_gen3x16_xdma_201920_3/krnl_vadd.xclbin
   Loading: './build_dir.hw.xilinx_u50_gen3x16_xdma_201920_3/krnl_vadd.xclbin'
   Trying to program device[0]: xilinx_u50_gen3x16_xdma_201920_3
   Device[0]: program successful!
   Running CASE 1  : Single HBM PC for all three Buffers 
   Each buffer is connected to same HBM PC.
   input 1 -> PC 0 
   input 2 -> PC 0 
   output  -> PC 0 
   [CASE 1] THROUGHPUT = 8.0181 GB/s
   Running CASE 2: Three Separate PCs for Three Buffers
   Each buffer is allocated with different HBM PC.
   input 1 -> PC 1 
   input 2 -> PC 2 
   output  -> PC 3 
   [CASE 2] THROUGHPUT = 24.4078 GB/s 
   TEST PASSED
