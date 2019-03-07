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
   Loop Fusion

   Loop fusion is a technique to combine a nested loop with its parent. This
   technique produces more efficient hardware in some cases.
  */

#define MAX_DIMS 5

// Tripcount identifiers
__constant int c_len = 512;
__constant int c_dim = 2;

 // This is a simple implementation of a linear search algorithm. We use two
 // loops. The outer loop cycles through each of the search space and the inner
 // loop calculates the distance to a particular point.
kernel __attribute__((reqd_work_group_size(1, 1, 1))) void
nearest_neighbor(global int *out, global const int *points,
                      global const int *search_point, const int len,
                      const int dim) {
    int best_i = 0;
    int best_dist = INT_MAX;
    int s_point[MAX_DIMS];

    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_dim, c_dim)))
    read: 
    for (int d = 0; d < dim; ++d) {
        s_point[d] = search_point[d];
    }

    __attribute__((xcl_loop_tripcount(c_len, c_len)))
    find_best:
    for (int p = 0; p < len; ++p) {
        int dist = 0;

        // Calculate the distance in a n-dimensional space
        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_dim, c_dim)))
        dist_calc:
        for (int c = 0; c < dim; ++c) {
            int dx = points[dim * p + c] - s_point[c];
            dist += dx * dx;
        }

        if (dist < best_dist) {
            best_i = p;
            best_dist = dist;
        }
    }

    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_dim, c_dim)))
    write_best:
    for (int c = 0; c < dim; ++c) {
        out[c] = points[best_i * dim + c];
    }
}

// This implementation fuses the distance calculation and the iteration through
// the search space into one loop.
kernel __attribute__((reqd_work_group_size(1, 1, 1))) void
nearest_neighbor_loop_fusion(global int *out, global const int *points,
                       global const int *search_point, const int len,
                       const int dim) {
    int best_i = 0;
    int best_dist = INT_MAX;
    int s_point[MAX_DIMS];

    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_dim, c_dim)))
    read: 
    for (int d = 0; d < dim; ++d) {
        s_point[d] = search_point[d];
    }

    int dist = 0;
    int iterations = len * dim;

    // This loop iterates through each point and through each of the dimension.
    // The combined loop performs the same number of iterations as the previous
    // implementation but this approach give the compiler more opportunity to
    // optimize the operations.
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_dim*c_len, c_dim*c_len)))
    find_best:
    for (int p = 0, c = 0, itr = 0; itr < iterations; itr++) {
        int dx = points[dim * p + c] - s_point[c];
        dist += dx * dx;
        // Defines the end of the dimension calculation(The inner loop in the
        // previous example)
        if (c == dim - 1) {
            if (dist < best_dist) {
                best_i = p;
                best_dist = dist;
            }
            c = 0;
            dist = 0;
            p++;
        } else {
            c++;
        }
    }
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_dim, c_dim)))
    write_best:
    for (int c = 0; c < dim; ++c) {
        out[c] = points[best_i * dim + c];
    }
}
