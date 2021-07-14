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

#define MAX_DIM 64

// Tripcount identifiers
const int c_size = MAX_DIM;

extern "C" {
void mmult(int* c, int* a, const int* b, const int dim0, const int dim1) {
    int matA[MAX_DIM * MAX_DIM];
    int matB[MAX_DIM * MAX_DIM];

// Auto-pipeline is going to apply pipeline to these loops
mmult_readA:
    for (int i = 0; i < dim0 * dim1; ++i) {
#pragma HLS LOOP_TRIPCOUNT min = c_size* c_size max = c_size * c_size
        matA[i] = a[i];
    }

mmult_readB:
    for (int i = 0; i < dim0 * dim1; ++i) {
#pragma HLS LOOP_TRIPCOUNT min = c_size* c_size max = c_size * c_size
        matB[i] = b[i];
    }

mmult1:
    for (int j = 0; j < dim1; ++j) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
    mmult2:
        for (int i = 0; i < dim0; ++i) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
            int temp = 0;
        mmult3:
            for (int k = 0; k < dim1; ++k)
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
                temp += matA[k + i * dim0] * matB[j + k * dim0];

            c[i + j * dim0] = temp;
        }
    }
}
}
