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

#include <stdint.h>
#define DATA_WIDTH 16

extern "C" {
void dot_product_3(const uint32_t* a, const uint32_t* b, uint64_t* res, const int size, const int reps) {
// Using HLS_INERFACE pragma to place each buffer in a different bank
// Default value for port width i.e. 2 bundles with each having a width of
// 512

#pragma HLS INTERFACE m_axi port = a bundle = gmem0
#pragma HLS INTERFACE m_axi port = b bundle = gmem1
#pragma HLS INTERFACE m_axi port = res bundle = gmem0

loop_reps:
    for (int i = 0; i < reps; i++) {
    dot_product_outer:
        for (int j = 0; j < size; j += DATA_WIDTH) {
        dot_product_inner:
            for (int k = 0; k < DATA_WIDTH; k++) {
// As the outer loop is not a perfect loop
#pragma HLS loop_flatten off
                res[j + k] = a[j + k] * b[j + k];
            }
        }
    }
}
}
