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

/*
   In this example we will demonstrate ways to improve the throughput of a
   vector addition kernel using the HLS PIPELINE pragma.
  */

#define N 128
#define DATA_SIZE 1 << 10

// Tripcount identifiers
const int c_len = DATA_SIZE;
const int c_n = N;

// This kernel is accessing 2 global variables and storing the result into
// a third global variable. This type of access does not yield good
// performance because this kernel has only one global memory port.

extern "C" {
void vadd(int* c, const int* a, const int* b, const int len, int rep_count) {
count_loop:
    for (int i = 0; i < rep_count; i++) {
    vadd_loop:
        for (int x = 0; x < len; ++x) {
// As the outer loop is not a perfect loop
#pragma HLS loop_flatten off
#pragma HLS LOOP_TRIPCOUNT min = c_len max = c_len
            c[x] = a[x] + b[x];
        }
    }
}
}
