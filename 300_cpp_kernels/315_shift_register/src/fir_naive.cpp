/**********
Copyright (c) 2018, Xilinx, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/

/*
  Finite Impulse Response(FIR) Filter

  This example demonstrates how to perform a shift register operation to
  implement a Finite Impulse Response(FIR) filter.

  Shift register operation is the cascading of values of an array by one or more
  places. Here is an example of what a shift register operation looks like on an
  array of length four:

                     ___       ___      ___      ___
  1. shift_reg[N] : | A |  <- | B | <- | C | <- | D |
                     ---       ---      ---      ---
                     ___       ___      ___      ___
  2. shift_reg[N] : | B |  <- | C | <- | D | <- | D |
                     ---       ---      ---      ---

  Here each of the values are copied into the register on the left. This type of
  operation is useful when you want to work on a sliding window of data or when
  the data is being streamed into the kernel.

  The Xilinx compiler can recognize this type of operation into the appropriate
  hardware. For example, the previous illustration can be coded using the
  following loop:

  #define N 4

  __attribute__((opencl_unroll_hint))
  for(int i = 0; i < N-1; i++) {
      shift_reg[i] = shift_reg[i+1];
  }

  The compiler needs to know the number of registers at compile time so the
  definition of N must be a compile time variable.

*/

// Number of coefficient components
#define N_COEFF 11
#define min(x,y) ((x) < (y) ? (x) : (y))
// A naive implementation of the Finite Impulse Response filter.
extern "C"{
void fir_naive(int *output_r,
               int *signal_r,
               int *coeff,
               long signal_length) {
#pragma HLS INTERFACE m_axi port=output_r offset=slave bundle=gmem
#pragma HLS INTERFACE m_axi port=signal_r offset=slave bundle=gmem
#pragma HLS INTERFACE m_axi port=coeff offset=slave bundle=gmem
#pragma HLS INTERFACE s_axilite port=output_r bundle=control 
#pragma HLS INTERFACE s_axilite port=signal_r bundle=control 
#pragma HLS INTERFACE s_axilite port=coeff bundle=control 
#pragma HLS INTERFACE s_axilite port=signal_length bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

    int coeff_reg[N_COEFF];
    read_coef: 
    for (int i = 0 ; i < N_COEFF ; i++) 
    #pragma HLS PIPELINE II=1
        coeff_reg[i] = coeff[i];

    outer_loop:
    for (int j = 0; j < signal_length; j++) {
        int acc = 0;
        shift_loop:
        for (int i = min(j,N_COEFF-1); i >= 0; i--) {
        #pragma HLS PIPELINE II=1
            acc += signal_r[j-i] * coeff_reg[i];
        }
        output_r[j] = acc;
    }
}
}
