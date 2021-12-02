Loop Reordering (OpenCL Kernel)
===============================

This is a simple example of matrix multiplication (Row x Col) to demonstrate how to achieve better pipeline II factor by loop reordering.

**KEY CONCEPTS:** `Kernel Optimization <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Kernel-Optimization>`__, Loop reorder to improve II

**KEYWORDS:** `xcl_pipeline_loop <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/OpenCL-Attributes>`__, `xcl_array_partition <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/xcl_array_partition>`__

**EXCLUDED PLATFORMS:** 

 - All NoDMA Platforms, i.e u50 nodma etc

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/mmult.cl
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./cl_loop_reorder <mmult XCLBIN>

DETAILS
-------

This example demonstrates how changing the order of loop can improve the
performance of the kernel. In this case, loop reordering helps us
achieve better ``Initiation Interval``\ (II).

If matrix multiplication is performed with the commented code,
Initiation Interval achieved is 32.

With the reordered loop where j and k variables are interchanged as
shown below, II achieved is 1. Reordering avoids the adder tree for the
sum(output) of a single output element.

.. code:: cpp

    __attribute__((xcl_loop_tripcount(c_size, c_size)))
       lreorder1: for (int i = 0; i < size; i++) {
           __attribute__((xcl_pipeline_loop(1)))
           __attribute__((xcl_loop_tripcount(c_size, c_size)))
           lreorder2: for (int k = 0; k < size; k++) {
               __attribute__((xcl_loop_tripcount(c_size, c_size)))
               lreorder3: for (int j = 0; j < MAX_SIZE; j++) {
                   int result = (k == 0) ? 0 : temp_sum[j];
                   result += A[i][k] * B[k][j];
                   temp_sum[j] = result;
                   if (k== size -1) C[i][j] = result;
               }
           }
       }

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.