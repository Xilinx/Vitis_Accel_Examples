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

// Maximum Array Size
#define MAX_SIZE 16

// Tripcount identifiers
__constant int c_size = MAX_SIZE;

// Naive implementation of matrix multiplication 
// In this implementation array partition is not done
// Computes matrix multiply
// C = AxB, where A, B and C are square matrices of dimension (sizexsize)
kernel __attribute__((reqd_work_group_size(1, 1, 1)))
void matmul(const __global int *in1,  // Read-Only Matrix 1
           const __global int *in2,  // Read-Only Matrix 2
           __global int *out,        // Output Result
           int size) {               // Local memory to store input and output matrices
    // Local memory is implemented as BRAM memory blocks
    local int A[MAX_SIZE][MAX_SIZE];
    local int B[MAX_SIZE][MAX_SIZE];
    local int C[MAX_SIZE][MAX_SIZE];
    local int temp_sum[MAX_SIZE];

    // Burst reads on input matrices from global memory
    // Burst read for matrix A
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_size*c_size, c_size*c_size)))
    readA:
    for (int itr = 0, i = 0, j = 0; itr < size * size; itr++, j++) {
        if (j == size) {
            j = 0;
            i++;
        }
        A[i][j] = in1[itr];
    }

    // Burst read for matrix B
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_size*c_size, c_size*c_size)))
    readB:
    for (int itr = 0, i = 0, j = 0; itr < size * size; itr++, j++) {
        if (j == size) {
            j = 0;
            i++;
        }
        B[i][j] = in2[itr];
    }


    // Calculate matrix multiplication using local data buffer based on input size
    // and write results into local buffer for C
    __attribute__((xcl_loop_tripcount(c_size, c_size)))
    nopart1:
    for (int i = 0; i < size; i++) {
        __attribute__((xcl_pipeline_loop))
        __attribute__((xcl_loop_tripcount(c_size, c_size)))
        nopart2 :
        for (int k = 0; k < size; k++) {
            __attribute__((xcl_loop_tripcount(c_size, c_size)))
            nopart3:
            for (int j = 0; j < MAX_SIZE; j++) {
                int result = (k == 0) ? 0 : temp_sum[j];
                result += A[i][k] * B[k][j];
                temp_sum[j] = result;
                if (k == size - 1) C[i][j] = result;
            }
        }
    }

    // Burst write from output matrices to global memory
    // Burst write from matrix C
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_size*c_size, c_size*c_size)))
    writeC:
    for (int itr = 0, i = 0, j = 0; itr < size * size; itr++, j++) {
        if (j == size) {
            j = 0;
            i++;
        }
        out[itr] = C[i][j];
    }
}


// Matrix multiplication kernel 
// This kernel presents array partition concept
kernel __attribute__((reqd_work_group_size(1, 1, 1)))
void matmul_partition(const __global int *in1,  // Read-Only Matrix 1
           const __global int *in2,  // Read-Only Matrix 2
           __global int *out,        // Output Result
           int size) {               // Local memory to store input and output matrices
    // Local memory is implemented as BRAM memory blocks
    int A[MAX_SIZE][MAX_SIZE];
    
    // Partition Matrix B on 2nd dimension completely
    int B[MAX_SIZE][MAX_SIZE] __attribute__((xcl_array_partition(complete, 2)));
    
    // Partition Matrix C on 2nd dimension completely
    int C[MAX_SIZE][MAX_SIZE] __attribute__((xcl_array_partition(complete, 2)));

    // Partition Matrix temp_sum completely  
    int temp_sum[MAX_SIZE]    __attribute__((xcl_array_partition(complete, 1)));

    // Burst reads on input matrices from global memory
    // Burst read for matrix A
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_size*c_size, c_size*c_size)))
    readA:
    for (int itr = 0, i = 0, j = 0; itr < size * size; itr++, j++) {
        if (j == size) {
            j = 0;
            i++;
        }
        A[i][j] = in1[itr];
    }

    // Burst read for matrix B
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_size*c_size, c_size*c_size)))
    readB:
    for (int itr = 0, i = 0, j = 0; itr < size * size; itr++, j++) {
        if (j == size) {
            j = 0;
            i++;
        }
        B[i][j] = in2[itr];
    }

    // Performs matrix multiply over matrices A and B and stores the result
    // in C. All the matrices are square matrices of the form (size x size)
    // Calculate matrix multiplication using local data buffer based on input size
    // and write results into local buffer for C
    __attribute__((xcl_loop_tripcount(c_size, c_size)))
    arraypart1:
    for (int i = 0; i < size; i++) {
        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_size, c_size)))
        arraypart2 :
        for (int k = 0; k < size; k++) {
            __attribute__((xcl_loop_tripcount(c_size, c_size)))
            arraypart3:
            for (int j = 0; j < MAX_SIZE; j++) {
                int result = (k == 0) ? 0 : temp_sum[j];
                result += A[i][k] * B[k][j];
                temp_sum[j] = result;
                if (k == size - 1) C[i][j] = result;
            }
        }
    }

    // Burst write from output matrices to global memory
    // Burst write from matrix C
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_size*c_size, c_size*c_size)))
    writeC:
    for (int itr = 0, i = 0, j = 0; itr < size * size; itr++, j++) {
        if (j == size) {
            j = 0;
            i++;
        }
        out[itr] = C[i][j];
    }
}

