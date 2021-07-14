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

//------------------------------------------------------------------------------
//
// kernel:  vadd
//
// Purpose: Demonstrate Vector Add in C++ HLS
//

#define LENGTH 256
#define BUFFER_SIZE 256

// Tripcount identifiers
const int c_size = BUFFER_SIZE;
const int c_len = LENGTH / BUFFER_SIZE;

extern "C" {
void krnl_vadd(int* a, int* e, const int length_r) {
    int arrayA[BUFFER_SIZE];
    int arrayB[BUFFER_SIZE];
    int arrayC[BUFFER_SIZE];
    int arrayD[BUFFER_SIZE];

    for (int i = 0; i < length_r; i += BUFFER_SIZE) {
#pragma HLS LOOP_TRIPCOUNT min = c_len max = c_len
        int size = BUFFER_SIZE;
        if (i + size > length_r) size = length_r - i;
    // Auto-pipeline is going to apply pipeline to these loops
    readA:
        for (int j = 0; j < 4 * size; j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
            int tmpValue = a[i + j];
            switch (j % 4) {
                case 0:
                    arrayA[j / 4] = tmpValue;
                    break;
                case 1:
                    arrayB[j / 4] = tmpValue;
                    break;
                case 2:
                    arrayC[j / 4] = tmpValue;
                    break;
                case 3:
                    arrayD[j / 4] = tmpValue;
                    break;
            }
        }

    vadd_writeC:
        for (int j = 0; j < size; j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
            e[j] = arrayA[j] + arrayB[j] + arrayC[j] + arrayD[j];
        }
    }
}
}
