/**
* Copyright (C) 2019-2021 Xilinx, Inc
*
* Licensed under the Apache License, Version 2.0 (the "License"). You may
* not use this file except in compliance with the License. A copy of the
* License is located at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
* License for the specific language governing permissions and limitations
* under the License.
*/

/*
   Loop Pipelining

   Pipelining is a form of parallelism that allows the FPGA to reuse the
   hardware for the next instruction before the current instruction has finished
   processing. This maximizes the utilization of the FPGA fabric and allows the
   processing of multiple elements of data at the same time.

   The V++ compiler automatically performs this optimization on loops so no
   user intervention is required for most cases. The outer most loops will be
   pipelined and inner loops will be unrolled if possible.

   There are instances where this technique will not yield the best performance.
   For example, if you access two or more global cl_mem objects from within a
   pipelined loop. By default V++ will assign one global memory port to each
   kernel. Because the operations within a pipelined loop are executing in
   parallel different stages of the pipeline will need access to the memory
   port. In order to accommodate this design, V++ will need to serialize the
   access to these ports and therefore increase the initiation interval(II)
   and by extension reduce the throughput.
  */

#define N 128
#define DATA_SIZE 1 << 10

// Tripcount identifiers
const int c_len = DATA_SIZE;
const int c_n = N;

// This kernel is optimized to access only one global variable in a pipelined
// loop. This will improve the II and increase throughput of the kernel.

extern "C" {
void vadd_pipelined(int* c, const int* a, const int* b, const int len, int rep_count) {
    int result[N];
    int iterations = len / N;

// Default behavior of V++ will pipeline the outer loop. Since we have
// multiple inner loops, the pipelining will fail. We can instead pipeline
// the inner loops using the HLS PIPELINE pragma to guide the compiler.
loop_count:
    for (int k = 0; k < rep_count; k++) {
    vadd_pipeline:
        for (int i = 0; i < iterations; i++) {
// As the outer loop is not a perfect loop
#pragma HLS loop_flatten off
#pragma HLS LOOP_TRIPCOUNT min = c_len / c_n max = c_len / c_n

        // Pipelining loops that access only one variable is the ideal way to
        // increase the global memory bandwidth.
        read_a:
            for (int x = 0; x < N; ++x) {
#pragma HLS LOOP_TRIPCOUNT min = c_n max = c_n
#pragma HLS PIPELINE II = 1
                result[x] = a[i * N + x];
            }

        read_b:
            for (int x = 0; x < N; ++x) {
#pragma HLS LOOP_TRIPCOUNT min = c_n max = c_n
#pragma HLS PIPELINE II = 1
                result[x] += b[i * N + x];
            }

        write_c:
            for (int x = 0; x < N; ++x) {
#pragma HLS LOOP_TRIPCOUNT min = c_n max = c_n
#pragma HLS PIPELINE II = 1
                c[i * N + x] = result[x];
            }
        }
    }
}
}
