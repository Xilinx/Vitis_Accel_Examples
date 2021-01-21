HBM Simple XRT (XRT Native API's)
=================================

This is simple example to demonstrate how to use HBM Platform. 
Three separate banks are used for each argument of kernels. Individual
bank for each buffer will provide more bandwidth for the application.


compute unit interfaces are associated to fixed HBM banks using
``krnl_vadd.cfg`` file:

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
real log reported while running on U50 platform:

::

   Open the device0
   Load the xclbin krnl_vadd.xclbin
   Running CASE 1  : Single HBM for all three Buffers 
   input 1 -> bank 0 
   input 2 -> bank 0 
   output  -> bank 0 
   Allocate Buffer in Global Memory
   synchronize input buffer data to device global memory
   Execution of the kernel
   Get the output data from the device
   [CASE 1] THROUGHPUT = 11.1863 GB/s
   Running CASE 2: Three Separate Banks for Three Buffers
   input 1 -> bank 1 
   input 2 -> bank 2 
   output  -> bank 3 
   Allocate Buffer in Global Memory
   synchronize input buffer data to device global memory
   Execution of the kernel
   Get the output data from the device
   [CASE 2] THROUGHPUT = 30.915 GB/s 
   TEST PASSED
   
