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
   Loop Pipelining

   Pipelining is a form of parallelism that allows the FPGA to reuse the
   hardware for the next instruction before the current instruction has finished
   processing. This maximizes the utilization of the FPGA fabric and allows the
   processing of multiple elements of data at the same time.

   The xocc compiler automatically performs this optimization on loops so no
   user intervention is required for most cases. The outer most loops will be
   pipelined and inner loops will be unrolled if possible.

   There are instances where this technique will not yield the best performance.
   For example, if you access two or more global cl_mem objects from within a
   pipelined loop. By default xocc will assign one global memory port to each
   kernel. Because the operations within a pipelined loop are executing in
   parallel different stages of the pipeline will need access to the memory
   port. In order to accommodate this design, xocc will need to serialize the
   access to these ports and therefore increase the initiation interval(II)
   and by extension reduce the throughput.

   In this example we will demonstrate ways to improve the throughput of a
   vector addition kernel using the xcl_pipeline_loop attribute.
  */

#define N 128
#define DATA_SIZE 1<<10

// Tripcount identifiers
__constant int c_len = DATA_SIZE;
__constant int c_n = N;

 // This kernel is accessing 2 global variables and storing the result into
 // a third global variable. This type of access does not yield good
 // performance because this kernel has only one global memory port.
kernel __attribute__((reqd_work_group_size(1, 1, 1)))
void vadd(global       int* restrict c,
          global const int* restrict a,
          global const int* restrict b,
                 const int len)
{
    __attribute__((xcl_loop_tripcount(c_len, c_len)))
    vadd_loop:
    for (int x=0; x<len; ++x) {
        c[x] = a[x] + b[x];
    }
}

// This kernel is optimized to access only one global variable in a pipelined
// loop. This will improve the II and increase throughput of the kernel.
kernel __attribute__((reqd_work_group_size(1, 1, 1)))
void vadd_pipelined(global       int* restrict c,
                    global const int* restrict a,
                    global const int* restrict b,
                           const int len)
{
    int result[N];
    int iterations = len / N;

    // Default behavior of xocc will pipeline the outer loop. Since we have
    // multiple inner loops, the pipelining will fail. We can instead pipeline
    // the inner loops using the xcl_pipeline_loop attribute to guide the
    // compiler.
    __attribute__((xcl_loop_tripcount(c_len/c_n, c_len/c_n)))
    for(int i = 0; i < iterations; i++) {

        // Pipelining loops that access only one variable is the ideal way to
        // increase the global memory bandwidth.
        read_a:
        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_n, c_n)))
        for (int x=0; x<N; ++x) {
            result[x] = a[i*N+x];
        }
        read_b:
        __attribute__((xcl_pipeline_loop))
        __attribute__((xcl_loop_tripcount(c_n, c_n)))
        for (int x=0; x<N; ++x) {
            result[x] += b[i*N+x];
        }
        write:
        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_n, c_n)))
        for (int x=0; x<N; ++x) {
            c[i*N+x] = result[x];
        }
    }
}
