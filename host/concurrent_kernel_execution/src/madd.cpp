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

#define MAX_DIM 64

// Tripcount identifiers
const int c_size = MAX_DIM;

extern "C" {
void madd(int *c, int *a, int *b, const int dim0, const int dim1) {

  int matA[MAX_DIM * MAX_DIM];
  int matB[MAX_DIM * MAX_DIM];

// Auto-pipeline is going to apply pipeline to these loops
madd_readA:
  for (int i = 0; i < dim0 * dim1; ++i) {
#pragma HLS LOOP_TRIPCOUNT min = c_size*c_size max = c_size*c_size
    matA[i] = a[i];
  }

madd_readB:
  for (int i = 0; i < dim0 * dim1; ++i) {
#pragma HLS LOOP_TRIPCOUNT min = c_size*c_size max = c_size*c_size
    matB[i] = b[i];
  }

madd_writeC:
  for (int i = 0; i < dim0 * dim1; ++i) {
#pragma HLS LOOP_TRIPCOUNT min = c_size*c_size max = c_size*c_size
    c[i] = matA[i] + matB[i];
  }
}
}
