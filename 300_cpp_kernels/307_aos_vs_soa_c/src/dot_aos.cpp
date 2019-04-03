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

#define N 32
#define VERTEX_COUNT 512

// Tripcount identifiers
const int c_iter = VERTEX_COUNT/N;
const int c_n = N;

// Structure for accessing 96bit data
typedef struct aos_struct {
    int var1;
    int var2; 
    int var3; 
    int pad;  // This element is used for alignment purposes
} int3;

/* Array of Structure: Regular Implementation*/
// For dot_aos, memory is organized point by point as below:
// p0x  p0y  p0z 
// p1x  p1y  p1z
// p2x  p2y  p2z
// p3x  p3y  p3z
// ...    ...    ...
// pNx  pNy  pNz 

extern "C" {
void dot_aos(int* result, 
             const int3* points,
             const int num_vertices) {
#pragma HLS INTERFACE m_axi port=result  offset=slave bundle=gmem
#pragma HLS INTERFACE m_axi port=points offset=slave bundle=gmem
#pragma HLS INTERFACE s_axilite port=result  bundle=control
#pragma HLS INTERFACE s_axilite port=points bundle=control
#pragma HLS INTERFACE s_axilite port=num_vertices  bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

#pragma HLS DATA_PACK variable=points 

    int3 lpts[N];
    int out[N];

    int iterations = num_vertices / N;
    for (int i = 0; i < iterations; i++) {
    #pragma HLS LOOP_TRIPCOUNT min=c_iter max=c_iter
        load_pts: for (int j = 0; j < N; j++) {
        #pragma HLS LOOP_TRIPCOUNT min=c_n max=c_n
        #pragma HLS PIPELINE II=1
            int3 tmp = points[i*N+j];
            
            lpts[j].var1 = tmp.var1;
            lpts[j].var2 = tmp.var2;
            lpts[j].var3 = tmp.var3;
        }

        compute: for (int j = 0; j < N; j++) {
        #pragma HLS LOOP_TRIPCOUNT min=c_n max=c_n
        #pragma HLS PIPELINE II=1
            int x = lpts[j].var1; 
            int y = lpts[j].var2; 
            int z = lpts[j].var3; 
            out[j] = x*x + y*y + z*z;
        }

        store_r: for (int j = 0; j < N; j++)
        #pragma HLS LOOP_TRIPCOUNT min=c_n max=c_n
        #pragma HLS UNROLL
            result[i*N+j] = out[j];
    }
}
}
