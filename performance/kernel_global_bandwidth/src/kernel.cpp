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
#include <ap_int.h>
#include <iostream>

auto constexpr DATAWIDTH = 512;
using TYPE = ap_uint<DATAWIDTH>;

// Tripcount identifiers
auto constexpr c_min_size = (1024 * 1024) / 64;
auto constexpr c_max_size = (1024 * 1024 * 1024) / 64;

extern "C" {
void bandwidth(TYPE* __restrict__ input0,
               TYPE* __restrict__ output0,
#if NDDR_BANKS == 3
               TYPE* __restrict__ output1,
#elif NDDR_BANKS > 3
               TYPE* __restrict__ input1,
               TYPE* __restrict__ output1,
#endif
               int64_t num_blocks) {

    for (int64_t blockindex = 0; blockindex < num_blocks; blockindex++) {
#pragma HLS LOOP_TRIPCOUNT min = c_min_size max = c_max_size
        TYPE temp0 = input0[blockindex];
        output0[blockindex] = temp0;
#if NDDR_BANKS == 3
        output1[blockindex] = temp0;
#elif NDDR_BANKS > 3
        TYPE temp1 = input1[blockindex];
        output1[blockindex] = temp1;
#endif
    }
}
}
