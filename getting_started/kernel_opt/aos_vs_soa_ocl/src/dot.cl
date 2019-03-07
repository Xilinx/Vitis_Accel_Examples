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
__constant int c_iter = VERTEX_COUNT/N;
__constant int c_n = N;

/* Array of Structure: Regular Implementation*/
// For dot_aos, memory is organized point by point as below:
// p0x  p0y  p0z 
// p1x  p1y  p1z
// p2x  p2y  p2z
// p3x  p3y  p3z
// ...    ...    ...
// pNx  pNy  pNz 
kernel __attribute__((reqd_work_group_size(1, 1, 1)))
void dot_aos(global       int*  restrict result,
             global const int3* restrict points,
                    const int num_vertices) {
    int3 lpts[N];
    int out[N];

    int iterations = num_vertices / N;
    __attribute__((xcl_loop_tripcount(c_iter, c_iter)))
    for (int i = 0; i < iterations; i++) {
        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_n, c_n)))
        load_pts:
        for (int j = 0; j < N; j++)
            lpts[j] = points[i*N+j];

        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_n, c_n)))
        compute:
        for (int j = 0; j < N; j++)
            out[j] = lpts[j].x * lpts[j].x + lpts[j].y * lpts[j].y + lpts[j].z * lpts[j].z;

        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_n, c_n)))
        store_r:
        for (int j = 0; j < N; j++)
            result[i*N+j] = out[j];
    }
}

/* Structure of Array: Optimized version of dot implementation */
//For dot_soa, three separate memory location is allocated for each 
//dimension as below:
// X_memory : p0x p1x p2x p3x .... pNx
// Y_memory : p0y p1y p2y p3y .... pNy
// Z_memory : p0z p1z p2z p3z .... pNz
// Re-organizing memory in host will improve the kernel performance,
// using this memory architecture kernel can increase the memory datawidth
// to int16 (512bit) is bigger than int3(96bit) so will provide faster access.
// int16 vector operation allowed to do 16 points parallel execution.
kernel __attribute__((reqd_work_group_size(1, 1, 1)))
void dot_soa(global       int16 * restrict result,
             global const int16 * restrict X,
             global const int16 * restrict Y,
             global const int16 * restrict Z,
             const int num_vertices) {
  // Because consecutive elements in memory we can load 16 values at once. This
  // improves the global memory bus utilization.
  int16 lx[N];
  int16 ly[N];
  int16 lz[N];
  int16 out[N];

  int iterations = num_vertices / (16*N);
  __attribute__((xcl_loop_tripcount(c_iter/16, c_iter/16)))
  for (int i = 0; i < iterations; i++) {
      __attribute__((xcl_pipeline_loop(1)))
      __attribute__((xcl_loop_tripcount(c_n, c_n)))
      load_x: for(int j = 0; j < N; j++) lx[j] = X[i*N+j];
      __attribute__((xcl_pipeline_loop(1)))
      __attribute__((xcl_loop_tripcount(c_n, c_n)))
      load_y: for(int j = 0; j < N; j++) ly[j] = Y[i*N+j];
      __attribute__((xcl_pipeline_loop(1)))
      __attribute__((xcl_loop_tripcount(c_n, c_n)))
      load_z: for(int j = 0; j < N; j++) lz[j] = Z[i*N+j];

      __attribute__((xcl_pipeline_loop(1)))
      __attribute__((xcl_loop_tripcount(c_n, c_n)))
      compute: for(int j = 0; j < N; j++) out[j] = lx[j] * lx[j] + ly[j] * ly[j] + lz[j] * lz[j];

      __attribute__((xcl_pipeline_loop(1)))
      __attribute__((xcl_loop_tripcount(c_n, c_n)))
      store_r: for(int j = 0; j < N; j++) result[i*N+j] = out[j];
  }
}
