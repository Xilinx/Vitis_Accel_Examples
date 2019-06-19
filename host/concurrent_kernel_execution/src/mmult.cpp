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

#define MAX_DIM 64

//Tripcount identifiers
const int c_size = MAX_DIM;

extern "C" {
void mmult(int *c, int *a, const int *b, const int dim0, const int dim1) {
#pragma HLS INTERFACE m_axi port = c offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = a offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = b offset = slave bundle = gmem
#pragma HLS INTERFACE s_axilite port = c bundle = control
#pragma HLS INTERFACE s_axilite port = a bundle = control
#pragma HLS INTERFACE s_axilite port = b bundle = control
#pragma HLS INTERFACE s_axilite port = dim0 bundle = control
#pragma HLS INTERFACE s_axilite port = dim1 bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

    int matA[MAX_DIM * MAX_DIM];
    int matB[MAX_DIM * MAX_DIM];

mmult_readA:
    for (int i = 0; i < dim0 * dim1; ++i) {
       #pragma HLS PIPELINE II=1
       #pragma HLS LOOP_TRIPCOUNT min=c_size*c_size max=c_size*c_size
        matA[i] = a[i];
    }

mmult_readB:
    for (int i = 0; i < dim0 * dim1; ++i) {
       #pragma HLS PIPELINE II=1
       #pragma HLS LOOP_TRIPCOUNT min=c_size*c_size max=c_size*c_size
        matB[i] = b[i];
    }

mmult1:
    for (int j = 0; j < dim1; ++j) {
       #pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
    mmult2:
        for (int i = 0; i < dim0; ++i) {
           #pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
            int temp = 0;
        mmult3:
            for (int k = 0; k < dim1; ++k)
               #pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
               #pragma HLS PIPELINE II=1
                temp += matA[k + i * dim0] * matB[j + k * dim0];

            c[i + j * dim0] = temp;
        }
    }
}
}
