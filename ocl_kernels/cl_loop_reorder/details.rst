Loop Reordering (OpenCL Kernel)
===============================

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
