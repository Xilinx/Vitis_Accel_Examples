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

#define SIGNAL_SIZE (1024*1024)

// Tripcount identifiers
__constant int c_n = N_COEFF;
__constant int c_size = SIGNAL_SIZE;

// A naive implementation of the Finite Impulse Response filter.
__kernel __attribute__ ((reqd_work_group_size(1, 1, 1)))
void fir_naive(__global int* restrict output_r,
               __global int* restrict signal_r,
               __global int* restrict coeff,
               long signal_length) {

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
}

// FIR using shift register
__kernel __attribute__ ((reqd_work_group_size(1, 1, 1)))
void fir_shift_register(__global int* restrict output_r,
                        __global int* restrict signal_r,
                        __global int* restrict coeff,
                        long signal_length) {
    int coeff_reg[N_COEFF];

    // Partitioning of this array is required because the shift register
    // operation will need access to each of the values of the array in
    // the same clock. Without partitioning the operation will need to
    // be performed over multiple cycles because of the limited memory
    // ports available to the array.
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

        // This is the shift register operation. The N_COEFF variable is defined
        // at compile time so the compiler knows the number of operations
        // performed by the loop. This loop does not require the unroll
        // attribute because the outer loop will be pipelined so
        // the compiler will unroll this loop in the process.
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
}
