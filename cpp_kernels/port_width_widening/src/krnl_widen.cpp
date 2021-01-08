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

#define size 512

extern "C" {
void krnl_widen(const int* a, const int* b, int* res) {
    int X_accum = 0;
    int Y_accum = 0;
    int i, j;
    int X_aux[size];
    int y_aux[size];

SUM_X:
    for (i = 0; i < size; i++) {
        X_accum += a[i];
        X_aux[i] = X_accum;
    }

SUM_Y:
    for (i = 0; i < size; i++) {
        Y_accum += b[i];
        y_aux[i] = Y_accum;
    }

MUL:
    for (i = 0; i < size; i++) {
        res[i] = X_aux[i] * y_aux[i];
    }
}
}
