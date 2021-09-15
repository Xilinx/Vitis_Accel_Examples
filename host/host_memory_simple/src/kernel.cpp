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

extern "C" {
void krnl_vadd(int* a, int* b, int* out, const int n_elements) {
    int arrayA[BUFFER_SIZE];
    for (int i = 0; i < n_elements; i += BUFFER_SIZE) {
        int size = BUFFER_SIZE;
        // boundary check
        if (i + size > n_elements) size = n_elements - i;

    // Burst reading A
    readA:
        for (int j = 0; j < size; j++) arrayA[j] = a[i + j];

    // Burst reading B and calculating C and Burst writing
    // to  Global memory
    vadd_writeC:
        for (int j = 0; j < size; j++) out[i + j] = arrayA[j] + b[i + j];
    }
}
}
