/**
* Copyright (C) 2020 Xilinx, Inc
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

/*******************************************************************************

Vitis Key Concept :

    This is a matrix multiplication based example which showcases the how
    reordering the loops helps achieve better pipeline initiation interval(II)
    and better performance.

*******************************************************************************/

/*

Kernel Description :

    Matrix multiply for matrices upto sizes (MAX_SIZE x MAX_SIZE) [MAX_SIZE
defined below].
    This example showcases how reordering the loops helps achieve a better
    pipeline initiation interval (II) and better performance.

    Arguments :

        int *in1   (input)     --> Input  Matrix 1
        int *in2   (input)     --> Input  Matrix 2
        int *out_r   (output)    --> Output Matrix
        int  size  (input)     --> Size of one dimension of the matrices

    Kernel Configuration :

        Matrices of upto size (MAX_SIZE x MAX_SIZE) [MAX_SIZE = 64 defined
below]
*/

// Maximum Array Size
#define MAX_SIZE 32

// Tripcount identifiers
__constant int c_size = MAX_SIZE;

// Computes matrix multiply
// C = AxB, where A, B and C are square matrices of dimension (sizexsize)
kernel __attribute__((reqd_work_group_size(1, 1, 1))) void mmult(
    const __global int* in1, // Read-Only Matrix 1
    const __global int* in2, // Read-Only Matrix 2
    __global int* out_r,     // Output Result
    int size) {              // Local memory to store input and output matrices
    // Local memory is implemented as BRAM memory blocks
    local int A[MAX_SIZE][MAX_SIZE];
    local int B[MAX_SIZE][MAX_SIZE] __attribute__((xcl_array_partition(complete, 2)));
    local int C[MAX_SIZE][MAX_SIZE] __attribute__((xcl_array_partition(complete, 2)));
    int temp_sum[MAX_SIZE] __attribute__((xcl_array_partition(complete, 1)));

    // Burst reads on input matrices from global memory
    // Burst read for matrix A
    __attribute__((xcl_pipeline_loop(1))) __attribute__((xcl_loop_tripcount(c_size * c_size, c_size * c_size))) readA
        : for (int itr = 0, i = 0, j = 0; itr < size * size; itr++, j++) {
        if (j == size) {
            j = 0;
            i++;
        }
        A[i][j] = in1[itr];
    }

    // Burst read for matrix B
    __attribute__((xcl_pipeline_loop(1))) __attribute__((xcl_loop_tripcount(c_size * c_size, c_size * c_size))) readB
        : for (int itr = 0, i = 0, j = 0; itr < size * size; itr++, j++) {
        if (j == size) {
            j = 0;
            i++;
        }
        B[i][j] = in2[itr];
    }

    // Performs matrix multiply over matrices A and B and stores the result
    // in C. All the matrices are square matrices of the form (size x size)

    // Pipeline attribute is specified for the innermost loop (lreorder3)
    // and the order of the loops lreorder2 and lreorder 3 are changed here.

    // When the iteration variables j and k are interchanged between the loops,
    // lreoder2 and lreorder3, the pipeline initiation interval (II) improves
    // and becomes 1 (ideal).

    // Also the reordering avoids creating an adder tree for calculating the
    // sum(output) of a single output element

    // lreorder1: for (int i = 0; i < size; i++) {
    //     lreorder2: for (int j = 0; j < size; j++) {
    //     __attribute__((xcl_pipeline_loop))
    //         lreorder3: for (int k = 0; k < MAX_SIZE; k++) {
    //             int result = (k == 0) ? 0 : temp_sum[j];
    //             result += A[i][k] * B[k][j];
    //             temp_sum[j] = result;
    //             if (k== size -1) C[i][j] = result;
    //         }
    //     }
    // }

    // The above code snippet of the Matrix Multiply kernel in which the loops
    // lreorder2 and lreorder3 are not interchanged, gives a pipeline initiation
    // interval (II) of 32

    // Calculate matrix multiplication using local data buffer based on input size
    // and write results into local buffer for C
    __attribute__((xcl_loop_tripcount(c_size, c_size))) lreorder1 : for (int i = 0; i < size; i++) {
        __attribute__((xcl_pipeline_loop(1))) __attribute__((xcl_loop_tripcount(c_size, c_size))) lreorder2
            : for (int k = 0; k < size; k++) {
            __attribute__((xcl_loop_tripcount(c_size, c_size))) lreorder3 : for (int j = 0; j < MAX_SIZE; j++) {
                int result = (k == 0) ? 0 : temp_sum[j];
                result += A[i][k] * B[k][j];
                temp_sum[j] = result;
                if (k == size - 1) C[i][j] = result;
            }
        }
    }

    // Burst write from output matrices to global memory
    // Burst write from matrix C
    __attribute__((xcl_pipeline_loop(1))) __attribute__((xcl_loop_tripcount(c_size * c_size, c_size * c_size))) writeC
        : for (int itr = 0, i = 0, j = 0; itr < size * size; itr++, j++) {
        if (j == size) {
            j = 0;
            i++;
        }
        out_r[itr] = C[i][j];
    }
}
