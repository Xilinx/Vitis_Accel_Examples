Loop Reordering (C)
===================

This is a simple example of matrix multiplication (Row x Col) to demonstrate how to achieve better pipeline II factor by loop reordering.

**KEY CONCEPTS:** `Kernel Optimization <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitis_hls_optimization_techniques.html>`__, Loop reorder to improve II

**KEYWORDS:** `#pragma HLS ARRAY_PARTITION <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/hls_pragmas.html#gle1504034361378>`__

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   nodma

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/mmult.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./loop_reorder <mmult XCLBIN>

DETAILS
-------

This example demonstrates how changing the order of loop can improve the
performance of the kernel. In this case, loop reordering helps us
achieve better ``Initiation Interval``\ (II).

If matrix multiplication is performed with the commented code as shown
below, Initiation Interval achieved is 64.

.. code:: cpp

   // lreorder1: for (int i = 0; i < size; i++) {
   //     lreorder2: for (int j = 0; j < size; j++) {
   //         lreorder3: for (int k = 0; k < MAX_SIZE; k++) {
   //             int result = (k == 0) ? 0 : temp_sum[j];
   //             result += A[i][k] * B[k][j];
   //             temp_sum[j] = result;
   //             if (k== size -1) C[i][j] = result;
   //         }
   //     }
   // }

With the reordered loop as shown below, II achieved is 1. Reordering
avoids the adder tree for the sum(output) of a single output element.

.. code:: cpp

   lreorder1:
       for (int i = 0; i < size; i++) {
          #pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
       lreorder2:
           for (int k = 0; k < size; k++) {
              #pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
           lreorder3:
               for (int j = 0; j < MAX_SIZE; j++) {
                   int result = (k == 0) ? 0 : temp_sum[j];
                   result += A[i][k] * B[k][j];
                   temp_sum[j] = result;
                   if (k == size - 1)
                       C[i][j] = result;
               }
           }
       }

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.