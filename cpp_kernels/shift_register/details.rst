Shift Register
==============

This example demonstrates two ways to implement an FIR filter discrete
convolution operation. It requires the values of all elements of FIR
filter array for each element of the output array.

In the naive implementation, limited memory ports are available from the
``shift_reg`` array to the kernel interface thus efficient pipelining of
the operation is not possible.

.. code:: cpp

   int coeff_reg[N_COEFF];
   read_coef:
    for (int i = 0; i < N_COEFF; i++)
       #pragma HLS PIPELINE II=1
        coeff_reg[i] = coeff[i];

   outer_loop:
    for (int j = 0; j < signal_length; j++) {
        int acc = 0;
    shift_loop:
        for (int i = min(j, N_COEFF - 1); i >= 0; i--) {
           #pragma HLS PIPELINE II=1
            acc += signal_r[j - i] * coeff_reg[i];
        }
        output_r[j] = acc;
    }

In the ``fir_shift_register`` pragma ``HLS ARRAY_PARTITION`` is used to
provide all array values simultaneously by partitioning the array such
that each element has its own ports for read/write. Also, since
``outer loop`` uses ``pragma HLS PIPELINE``, ``Vivado HLS`` compiler
automatically applies ``loop unrolling`` to the ``shift_loop``.

.. code:: cpp

   int shift_reg[N_COEFF];
     #pragma HLS ARRAY_PARTITION variable=shift_reg complete dim=0

   init_loop:
      for (int i = 0; i < N_COEFF; i++) {
         #pragma HLS PIPELINE II=1
          shift_reg[i] = 0;
          coeff_reg[i] = coeff[i];
      }

   outer_loop:
      for (int j = 0; j < signal_length; j++) {
         #pragma HLS PIPELINE II=1
          int acc = 0;
          int x = signal_r[j];

   shift_loop:
          for (int i = N_COEFF - 1; i >= 0; i--) {
              if (i == 0) {
                  acc += x * coeff_reg[0];
                  shift_reg[0] = x;
              } else {
                  shift_reg[i] = shift_reg[i - 1];
                  acc += shift_reg[i] * coeff_reg[i];
              }
          }
          output_r[j] = acc;
      }

Following is the log reported while running the design on U200 platform:

::

   Platform Name: Xilinx
   INFO: Reading ./build_dir.hw.xilinx_u200_xdma_201830_2/fir.xclbin
   Loading: './build_dir.hw.xilinx_u200_xdma_201830_2/fir.xclbin'
   Example Testdata Signal_Length=1048576 for 100 iteration
   |-------------------------+-------------------------|
   | Kernel(100 iterations) | Wall-Clock Time (ns) |
   |-------------------------+-------------------------|
   | fir_naive | 85649415155 |
   | fir_shift_register | 12083476500 |
   |-------------------------+-------------------------|
   | Speedup: | 7.088143 |
   |-------------------------+-------------------------|
   Note: Wall Clock Time is meaningful for real hardware execution only, not for emulation.
   Please refer to profile summary for kernel execution time for hardware emulation.
   TEST PASSED
