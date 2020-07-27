Shift Register (OpenCL Kernel)
==============================

This example demonstrates two ways to implement an FIR filter discrete
convolution operation. It requires the values of all elements of FIR
filter array for each element of the output array.

In the naive implementation, limited memory ports are available from the
``shift_reg`` array to the kernel interface thus efficient pipelining of
the operation is not possible.

.. code:: cpp

   int coeff_reg[N_COEFF];
       __attribute__((xcl_loop_tripcount(c_n, c_n)))
       read_coef: for (int i = 0 ; i < N_COEFF ; i++) coeff_reg[i] = coeff[i];

       __attribute__((xcl_loop_tripcount(c_size, c_size)))
       outer_loop:
       for (int j = 0; j < signal_length; j++) {
           int acc = 0;
           shift_loop:
           __attribute__((xcl_pipeline_loop(1)))
           for (int i = min(j,N_COEFF-1); i >= 0; i--) {
               acc += signal_r[j-i] * coeff_reg[i];
           }
           output_r[j] = acc;
       }

In the ``fir_shift_register`` attribute ``xcl_array_partition`` is used
to provide all array values simultaneously by partitioning the array
such that each element has its own ports for read/write. Also, since
``outer loop`` uses attribute ``xcl_pipeline_loop``, ``Vivado HLS``
compiler automatically applies ``loop unrolling`` to the ``shift_loop``.

.. code:: cpp

    int shift_reg[N_COEFF] __attribute__((xcl_array_partition(complete, 0)));

       __attribute__((xcl_loop_tripcount(c_n, c_n)))
       init_loop:
       for (int i = 0; i < N_COEFF; i++) {
           shift_reg[i] = 0;
           coeff_reg[i] = coeff[i];
       }

       outer_loop:
       __attribute__((xcl_pipeline_loop(1)))
       __attribute__((xcl_loop_tripcount(c_size, c_size)))
       for(int j = 0; j < signal_length; j++) {
           int acc = 0;
           int x = signal_r[j];

           __attribute__((xcl_loop_tripcount(c_n, c_n)))
           shift_loop:
           for (int i = N_COEFF-1; i >= 0; i--) {
               if (i == 0) {
                   acc += x * coeff_reg[0];
                   shift_reg[0] = x;
               } else {
                   shift_reg[i] = shift_reg[i-1];
                   acc += shift_reg[i] * coeff_reg[i];
               }
           }
           output_r[j] = acc;
       }
