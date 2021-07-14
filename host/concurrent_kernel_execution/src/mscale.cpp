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
void mscale(int* inout_r, const int scale, const int dim0, const int dim1) {
    int temp[MAX_DIM * MAX_DIM];

// Auto-pipeline is going to apply pipeline to these loops
mscale:
    for (int i = 0; i < dim0 * dim1; ++i)
#pragma HLS LOOP_TRIPCOUNT min = c_size* c_size max = c_size * c_size
        temp[i] = inout_r[i] * scale;

mscale_write:
    for (int i = 0; i < dim0 * dim1; ++i)
#pragma HLS LOOP_TRIPCOUNT min = c_size* c_size max = c_size * c_size
        inout_r[i] = temp[i];
}
}
