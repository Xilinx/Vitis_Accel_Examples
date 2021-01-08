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

// Maximum Matrix Dimension Supported by Kernel
#define MAX_DIM 16

// TRIPCOUNT identifier
const unsigned int c_dim = MAX_DIM;

extern "C" {
// Computes matrix multiply
// C = AxB, where A, B and C are square matrices of dimension (dimxdim)
void matmul_naive(const int* in1, // Read-Only Matrix 1
                  const int* in2, // Read-Only Matrix 2
                  int* out_r,     // Output Result
                  int dim)        // Matrix Dimension (assuming square matrix)
{
#pragma HLS INTERFACE m_axi port = in1 offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = in2 offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = out_r offset = slave bundle = gmem
#pragma HLS INTERFACE s_axilite port = in1
#pragma HLS INTERFACE s_axilite port = in2
#pragma HLS INTERFACE s_axilite port = out_r
#pragma HLS INTERFACE s_axilite port = dim
#pragma HLS INTERFACE s_axilite port = return

    // Local memory is implemented as BRAM memory blocks
    int A[MAX_DIM * MAX_DIM];
    int B[MAX_DIM * MAX_DIM];
    int C[MAX_DIM * MAX_DIM];

// Burst read for matrix A
// Auto-pipeline is going to apply pipeline to these loops
readA:
    for (int i = 0; i < dim * dim; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_dim* c_dim max = c_dim * c_dim
        A[i] = in1[i];
    }

// Burst read for matrix B
readB:
    for (int i = 0; i < dim * dim; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_dim* c_dim max = c_dim * c_dim
        B[i] = in2[i];
    }

lreorder1:
    for (int i = 0; i < dim; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_dim max = c_dim
    lreorder2:
        for (int j = 0; j < MAX_DIM; j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_dim max = c_dim
            int result = 0;
        lreorder3:
            for (int k = 0; k < dim; k++) {
#pragma HLS LOOP_TRIPCOUNT min = c_dim max = c_dim
                result += A[i * dim + k] * B[k * dim + j];
            }
            C[i * dim + j] = result;
        }
    }

// Burst write from matrix C
writeC:
    for (int i = 0; i < dim * dim; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_dim* c_dim max = c_dim * c_dim
        out_r[i] = C[i];
    }
}
}
