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

#define BUFFER_SIZE 256
#define DATA_SIZE 1024

// TRIPCOUNT indentifier
const unsigned int c_len = DATA_SIZE / BUFFER_SIZE;
const unsigned int c_size = BUFFER_SIZE;

extern "C" {
void vadd(int* c, int* a, int* b, const int len, const int iter) {
    int arrayA[BUFFER_SIZE];
    int arrayB[BUFFER_SIZE];
    for (int i = 0; i < len; i += BUFFER_SIZE) {
#pragma HLS LOOP_TRIPCOUNT min = c_len max = c_len
        int size = BUFFER_SIZE;
        if (i + size > len) size = len - i;
    // Auto-pipeline is going to apply pipeline to these loops
    readA:
        for (int j = 0; j < size; j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
            arrayA[j] = a[i + j];
        }

    readB:
        for (int j = 0; j < size; j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
            arrayB[j] = b[i + j];
        }
    iterations:
        for (int itr = 0; itr < iter; itr++) {
        vadd_writeC:
            for (int j = 0; j < size; j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
                c[i + j] = arrayA[j] + arrayB[j];
            }
        }
    }
}
}
