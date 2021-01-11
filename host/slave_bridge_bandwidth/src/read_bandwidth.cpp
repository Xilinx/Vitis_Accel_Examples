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
#include <ap_int.h>
#include <iostream>

auto constexpr DATAWIDTH = 512;
using TYPE = ap_uint<DATAWIDTH>;

extern "C" {
void read_bandwidth(TYPE* input0, int64_t buf_size, int64_t iter) {
#pragma HLS INTERFACE m_axi port = input0 offset = slave bundle = gmem max_read_burst_length = \
    64 num_read_outstanding = 256
#pragma HLS INTERFACE s_axilite port = input0
#pragma HLS INTERFACE s_axilite port = buf_size
#pragma HLS INTERFACE s_axilite port = iter
#pragma HLS INTERFACE s_axilite port = return

    TYPE temp = 0;
    for (int64_t i = 0; i < iter; i++) {
        for (int64_t blockindex = 0; blockindex < buf_size; blockindex++) {
            temp |= input0[blockindex];
        }
    }
    input0[0] = temp;
}
}
