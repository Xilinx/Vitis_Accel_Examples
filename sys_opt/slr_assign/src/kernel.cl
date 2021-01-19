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

#define BUFFER_SIZE 256
kernel __attribute__((reqd_work_group_size(1, 1, 1))) void vmult(global const int* A,
                                                                 global const int* B,
                                                                 global int* C,
                                                                 const int n_elements) {
    int arrayA[BUFFER_SIZE];
    int arrayB[BUFFER_SIZE];
    for (int i = 0; i < n_elements; i += BUFFER_SIZE) {
        int size = BUFFER_SIZE;
        if (i + size > n_elements) size = n_elements - i;

        __attribute__((xcl_pipeline_loop(1))) readA : for (int j = 0; j < size; j++) arrayA[j] = A[i + j];

        __attribute__((xcl_pipeline_loop(1))) readB : for (int j = 0; j < size; j++) arrayB[j] = B[i + j];

        __attribute__((xcl_pipeline_loop(1))) vmult_writeC : for (int j = 0; j < size; j++) C[i + j] =
                                                                 arrayA[j] * arrayB[j];
    }
}

kernel __attribute__((reqd_work_group_size(1, 1, 1))) void vadd(global const int* D,
                                                                global const int* E,
                                                                global int* F,
                                                                const int n_elements) {
    int arrayD[BUFFER_SIZE];
    int arrayE[BUFFER_SIZE];
    for (int i = 0; i < n_elements; i += BUFFER_SIZE) {
        int size = BUFFER_SIZE;
        if (i + size > n_elements) size = n_elements - i;

        __attribute__((xcl_pipeline_loop(1))) readD : for (int j = 0; j < size; j++) arrayD[j] = D[i + j];

        __attribute__((xcl_pipeline_loop(1))) readE : for (int j = 0; j < size; j++) arrayE[j] = E[i + j];

        __attribute__((xcl_pipeline_loop(1))) vadd_writeF : for (int j = 0; j < size; j++) F[i + j] =
                                                                arrayD[j] + arrayE[j];
    }
}
