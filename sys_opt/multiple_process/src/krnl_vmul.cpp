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

#include "multi_krnl.h"

/*
    Vector Multiplication Kernel Implementation
    Arguments:
        a           (input)    --> Input Vector1
        b           (input)    --> Input Vector2
        out_r         (output)   --> Output Vector
        n_elements  (input)    --> Size of Vector in Integer
*/

extern "C" {
void krnl_vmul(int* a, int* b, int* out_r, const int n_elements) {
#pragma HLS INTERFACE m_axi port = a offset = slave bundle = gmem
#pragma HLS INTERFACE s_axilite port = a
#pragma HLS INTERFACE m_axi port = b offset = slave bundle = gmem
#pragma HLS INTERFACE s_axilite port = b
#pragma HLS INTERFACE m_axi port = out_r offset = slave bundle = gmem
#pragma HLS INTERFACE s_axilite port = out_r
#pragma HLS INTERFACE s_axilite port = n_elements
#pragma HLS INTERFACE s_axilite port = return

    int arrayA[BUFFER_SIZE];

vmul:
    for (int i = 0; i < n_elements; i += BUFFER_SIZE) {
#pragma HLS LOOP_TRIPCOUNT min = c_len max = c_len
        int size = BUFFER_SIZE;
        // boundary check
        if (i + size > n_elements) size = n_elements - i;

    // Burst reading A
    // Auto-pipeline is going to apply pipeline to these loops
    readA:
        for (int j = 0; j < size; j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
            arrayA[j] = a[i + j];
        }

    // Burst reading B and calculating C and Burst writing
    // to  Global memory
    vmul_writeC:
        for (int j = 0; j < size; j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
            out_r[i + j] = arrayA[j] * b[i + j];
        }
    }
}
}
