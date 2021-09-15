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

auto constexpr DATA_WIDTH = 512;
auto constexpr c_widthInBytes = DATA_WIDTH / 8;
auto constexpr c_maxBurstSize = 4 * 1024; // 4KB
auto constexpr c_burstLength = c_maxBurstSize / c_widthInBytes;

using TYPE = ap_uint<DATA_WIDTH>;

extern "C" {
void write_bandwidth(TYPE* output0, int64_t buf_size, int64_t iter) {
#pragma HLS INTERFACE m_axi port = output0 offset = slave bundle = gmem max_write_burst_length = \
    64 num_write_outstanding = 16
#pragma HLS INTERFACE s_axilite port = output0
#pragma HLS INTERFACE s_axilite port = buf_size
#pragma HLS INTERFACE s_axilite port = iter
#pragma HLS INTERFACE s_axilite port = return

    TYPE temp = 0;

    uint32_t factor = buf_size / c_maxBurstSize;
    uint32_t Indx = 0;
    uint32_t baseAddr = 0;

    if (buf_size <= 8 * 1024) {
    write_bandwidth_small_1:
        for (int itr = 0; itr < iter * factor; itr++) {
        write_bandwidth_small_2:
            for (int i = 0; i < c_burstLength; i++) {
                output0[baseAddr + i] = 1;
            }
            Indx = itr % factor;
            baseAddr = c_burstLength * Indx;
        }
    } else {
        buf_size = buf_size / c_widthInBytes;
    write_bandwidth_large_1:
        for (int64_t i = 0; i < iter; i++) {
        write_bandwidth_large_2:
            for (int64_t blockindex = 0; blockindex < buf_size; blockindex++) {
                output0[blockindex] = 1;
            }
        }
    }
}
}
