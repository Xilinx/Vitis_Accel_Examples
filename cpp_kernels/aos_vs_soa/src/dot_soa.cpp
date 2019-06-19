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
const int c_iter = VERTEX_COUNT / N;
const int c_n = N;

// Structure for accessing 512bit data
// Any arrays in the struct are partitioned into individual array elements
// and placed in the vector from lowest to highest, in order.

typedef struct soa_struct {
    int var[VERTEX_COUNT / N];
} int16;

/* Structure of Array: Optimized version of dot implementation */
//For dot_soa, three separate memory location is allocated for each
//dimension as below:
// X_memory : p0x p1x p2x p3x .... pNx
// Y_memory : p0y p1y p2y p3y .... pNy
// Z_memory : p0z p1z p2z p3z .... pNz
// Re-organizing memory in host will improve the kernel performance,
// using this memory architecture kernel can increase the memory datawidth
// to int16 (512bit) which is bigger than int3 (96bit) so will provide faster access.

extern "C" {
void dot_soa(int16 *result,
             const int16 *X,
             const int16 *Y,
             const int16 *Z,
             const int num_vertices) {
#pragma HLS INTERFACE m_axi port = result offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = X offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = Y offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = Z offset = slave bundle = gmem
#pragma HLS INTERFACE s_axilite port = result bundle = control
#pragma HLS INTERFACE s_axilite port = X bundle = control
#pragma HLS INTERFACE s_axilite port = Y bundle = control
#pragma HLS INTERFACE s_axilite port = Z bundle = control
#pragma HLS INTERFACE s_axilite port = num_vertices bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

#pragma HLS DATA_PACK variable = result
#pragma HLS DATA_PACK variable = X
#pragma HLS DATA_PACK variable = Y
#pragma HLS DATA_PACK variable = Z

    // Because consecutive elements in memory we can load 16 values at once. This
    // improves the global memory bus utilization.
    int16 lx[N];
    int16 ly[N];
    int16 lz[N];
    int16 out[N];

    int iterations = num_vertices / (16 * N);

    for (int i = 0; i < iterations; i++) {
       #pragma HLS LOOP_TRIPCOUNT min=c_iter max=c_iter

    load_x:
        for (int j = 0; j < N; j++) {
           #pragma HLS LOOP_TRIPCOUNT min=c_n max=c_n
           #pragma HLS PIPELINE II=1
            lx[j] = X[i * N + j];
        }

    load_y:
        for (int j = 0; j < N; j++) {
           #pragma HLS LOOP_TRIPCOUNT min=c_n max=c_n
           #pragma HLS PIPELINE II=1
            ly[j] = Y[i * N + j];
        }

    load_z:
        for (int j = 0; j < N; j++) {
           #pragma HLS LOOP_TRIPCOUNT min=c_n max=c_n
           #pragma HLS PIPELINE II=1
            lz[j] = Z[i * N + j];
        }

    compute:
        for (int j = 0; j < N; j++) {
           #pragma HLS LOOP_TRIPCOUNT min=c_n max=c_n
           #pragma HLS PIPELINE II=1
        compute2:
            for (int k = 0; k < VERTEX_COUNT / N; k++)
               #pragma HLS LOOP_TRIPCOUNT min=c_iter max=c_iter
                out[j].var[k] = lx[j].var[k] * lx[j].var[k] +
                                ly[j].var[k] * ly[j].var[k] +
                                lz[j].var[k] * lz[j].var[k];
        }

    store_r:
        for (int j = 0; j < N; j++) {
           #pragma HLS LOOP_TRIPCOUNT min=c_n max=c_n
           #pragma HLS PIPELINE II=1
            result[i * N + j] = out[j];
        }
    }
}
}
