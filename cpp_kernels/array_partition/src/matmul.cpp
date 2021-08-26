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

/*******************************************************************************
Description:
    C Kernel Example of Matrix Multiplication to demonstrate without array
partition.
*******************************************************************************/

// Includes
#include <stdio.h>
#include <string.h>

#define MAX_SIZE 16

// TRIPCOUNT identifiers
const unsigned int c_dim = MAX_SIZE;

extern "C" {
// Naive implementation of matrix multiplication
// In this implementation array partition is not done
void matmul(int* in1, int* in2, int* out_r, int size, int rep_count) {
    // Local buffers to hold input data
    int A[MAX_SIZE][MAX_SIZE];
    int B[MAX_SIZE][MAX_SIZE];
    int C[MAX_SIZE][MAX_SIZE];
    int temp_sum[MAX_SIZE];

// Read data from global memory and write into local buffer for in1
readA:
    for (int itr = 0, i = 0, j = 0; itr < size * size; itr++, j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_dim* c_dim max = c_dim * c_dim
        if (j == size) {
            j = 0;
            i++;
        }
        A[i][j] = in1[itr];
    }

// Read data from global memory and write into local buffer for in2
readB:
    for (int itr = 0, i = 0, j = 0; itr < size * size; itr++, j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_dim* c_dim max = c_dim * c_dim
        if (j == size) {
            j = 0;
            i++;
        }
        B[i][j] = in2[itr];
    }

// Calculate matrix multiplication using local data buffer based on input size,
// and write results into C
count_loop:
    for (int i = 0; i < rep_count; i++) {
    nopart1:
        for (int row = 0; row < size; row++) {
#pragma HLS LOOP_TRIPCOUNT min = c_dim max = c_dim
        nopart2:
            for (int col = 0; col < size; col++) {
#pragma HLS LOOP_TRIPCOUNT min = c_dim max = c_dim
            nopart3:
                for (int j = 0; j < MAX_SIZE; j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_dim max = c_dim
                    int result = (col == 0) ? 0 : temp_sum[j];
                    result += A[row][col] * B[col][j];
                    temp_sum[j] = result;
                    if (col == size - 1) C[row][j] = result;
                }
            }
        }
    }

// Write results from local buffer to global memory for out
writeC:
    for (int itr = 0, i = 0, j = 0; itr < size * size; itr++, j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_dim* c_dim max = c_dim * c_dim
        if (j == size) {
            j = 0;
            i++;
        }
        out_r[itr] = C[i][j];
    }
}
}
