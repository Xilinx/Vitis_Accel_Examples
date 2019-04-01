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

// Maximum Matrix Dimension Supported by Kernel
#define MAX_DIM 64

// Tripcount identifiers
__constant int c_size = MAX_DIM;

// Computes matrix multiply
// C = AxB, where A, B and C are square matrices of dimension (dimxdim)
kernel __attribute__((reqd_work_group_size(1, 1, 1)))
void matmul_naive(
            const __global int *in1,  // Read-Only Matrix 1
            const __global int *in2,  // Read-Only Matrix 2
            __global int *out_r,        // Output Result
            int dim)                  // Matrix Dimension (assuming square matrix)
{               
  // Local memory is implemented as BRAM memory blocks
    local int A[MAX_DIM * MAX_DIM];
    local int B[MAX_DIM * MAX_DIM];
    local int C[MAX_DIM * MAX_DIM];

    // Burst read for matrix A
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_size*c_size, c_size*c_size)))
    readA:
    for (int i = 0 ; i < dim * dim; i++) {
        A[i]  = in1[i];
    }

    // Burst read for matrix B
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_size*c_size, c_size*c_size)))
    readB:
    for (int i = 0 ; i < dim * dim; i++) {
        B[i]  = in2[i];
    }

    __attribute__((xcl_loop_tripcount(c_size, c_size)))
    lreorder1:
    for (int i = 0; i < dim; i++) {
        __attribute__((xcl_loop_tripcount(c_size, c_size)))
        lreorder2 :
        for (int j = 0; j < MAX_DIM; j++) {
            int result = 0;
            __attribute__((xcl_pipeline_loop(1)))
            __attribute__((xcl_loop_tripcount(c_size, c_size)))
            lreorder3:
            for (int k = 0; k < dim; k++) {
                result += A[i * dim +  k] * B[k * dim + j];
            }
            C[i*dim + j] = result;
        }
    }

    // Burst write from matrix C
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_size*c_size, c_size*c_size)))
    writeC:
    for (int i = 0; i < dim * dim; i++) {
        out_r[i] = C[i];
    }
}

kernel __attribute__((reqd_work_group_size(1, 1, 1)))
void matmul_partition(const __global int *in1,  // Read-Only Matrix 1
           const __global int *in2,  // Read-Only Matrix 2
           __global int *out_r,        // Output Result
           int dim) {                // Matrix Dimension. Assuming Square Matrix 

    //Cyclic Partition for A as matrix multiplication needs row-wise parallel access
    int A[MAX_DIM * MAX_DIM] __attribute__((xcl_array_partition(cyclic, MAX_DIM, 1)));
    //Block Partition for B as matrix multiplication needs column-wise parallel access
    int B[MAX_DIM * MAX_DIM] __attribute__((xcl_array_partition(block, MAX_DIM, 1)));
    int C[MAX_DIM * MAX_DIM]; 

    //As A and B Matrix are partitioned with the factor of MAX_DIM, so to get 
    // parallel row/column access, input square matrix[dimXdim] should be written
    // into local Array in MATRIX[MAX_DIM * MAX_DIM] format

    // Burst read for matrix A
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_size*c_size, c_size*c_size)))
    readA:
    for (int itr = 0, i = 0, j = 0; itr < dim * dim; itr++, j++) {
        if (j == dim) { j = 0; i++; }
        A[i*MAX_DIM + j] = in1[itr];
    }

    // Burst read for matrix B
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_size*c_size, c_size*c_size)))
    readB:
    for (int itr = 0, i = 0, j = 0; itr < dim * dim; itr++, j++) {
        if (j == dim) { j = 0; i++; }
        B[i * MAX_DIM + j] = in2[itr];
    }

    __attribute__((xcl_loop_tripcount(c_size, c_size)))
    lreorder1:
    for (int i = 0; i < dim; i++) {
        //As A and B are partition correctly so loop pipelining is applied
        // at 2nd level loop and which will eventually unroll the lower loop
        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_size, c_size)))
        lreorder2 :
        for (int j = 0; j < dim ; j++) {
            int result = 0;
            __attribute__((xcl_loop_tripcount(c_size, c_size)))
            lreorder3:
            for (int k = 0; k < MAX_DIM; k++) {
                result += A[i * MAX_DIM +  k] * B[k * MAX_DIM + j];
            }
            C[i*MAX_DIM + j] = result;
        }
    }

    // Burst write from output matrices to global memory
    // Burst write from matrix C
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_size*c_size, c_size*c_size)))
    writeC:
    for (int itr = 0, i = 0, j = 0; itr < dim * dim; itr++, j++) {
        if (j == dim) { j = 0; i++; }
        out_r[itr] = C[i * MAX_DIM + j];
    }
}

