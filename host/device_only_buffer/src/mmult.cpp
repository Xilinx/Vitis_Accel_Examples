/**********
Copyright (c) 2020, Xilinx, Inc.
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
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/

#include <stdio.h>
#include <string.h>

// Maximum Array Size
#define MAX_SIZE 32

// TRIPCOUNT indentifier
const unsigned int c_size = MAX_SIZE;

// Computes matrix multiply
// C = AxB, where A, B and C are square matrices of dimension (sizexsize)
extern "C" {
void mmult(const int *in1, // Read-Only Matrix 1
           const int *in2, // Read-Only Matrix 2
           int *out_r,     // Output Result
           int size        // Size of one dimension of the matrices
           ) {

  // Local memory to store input and output matrices
  // Local memory is implemented as BRAM memory blocks
  int A[MAX_SIZE][MAX_SIZE];
  int B[MAX_SIZE][MAX_SIZE];
  int C[MAX_SIZE][MAX_SIZE];
  int temp_sum[MAX_SIZE];
#pragma HLS ARRAY_PARTITION variable = B dim = 2 complete
#pragma HLS ARRAY_PARTITION variable = C dim = 2 complete
#pragma HLS ARRAY_PARTITION variable = temp_sum dim = 1 complete

// Burst reads on input matrices from global memory
// Burst read for matrix A
// Auto-pipeline is going to apply pipeline to these loops
readA:
  for (int itr = 0, i = 0, j = 0; itr < size * size; itr++, j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size*c_size max = c_size*c_size
    if (j == size) {
      j = 0;
      i++;
    }
    A[i][j] = in1[itr];
  }

// Burst read for matrix B
readB:
  for (int itr = 0, i = 0, j = 0; itr < size * size; itr++, j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size*c_size max = c_size*c_size
    if (j == size) {
      j = 0;
      i++;
    }
    B[i][j] = in2[itr];
  }

mmult1:
  for (int i = 0; i < size; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
  mmult2:
    for (int k = 0; k < size; k++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
    mmult3:
      for (int j = 0; j < MAX_SIZE; j++) {
        int result = (k == 0) ? 0 : temp_sum[j];
        result += A[i][k] * B[k][j];
        temp_sum[j] = result;
        if (k == size - 1)
          C[i][j] = result;
      }
    }
  }

// Burst write from output matrices to global memory
// Burst write from matrix C
writeC:
  for (int itr = 0, i = 0, j = 0; itr < size * size; itr++, j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size*c_size max = c_size*c_size
    if (j == size) {
      j = 0;
      i++;
    }
    out_r[itr] = C[i][j];
  }
}
}
