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

/*******************************************************************************
Description:
    One cycle of data process includes: p2p data transfer (p), kernel copy (c)
    and XDMA (x). Conceptually, x, c and p happens consecutively. The pipeline
    is designed so that c(n) and x(n) will happen in parallel with p(n+1), in
    which n is the nth chunk of data.
*******************************************************************************/

#include <ap_int.h>
#include <stdint.h>
#define SIZE 1024

typedef ap_uint<512> uintV_t;

void read(uintV_t* in, uintV_t buffer_in[SIZE]) {
// Auto-pipeline is going to apply pipeline to these loops
read:
    for (int i = 0; i < SIZE; i++) {
        buffer_in[i] = in[i];
    }
}

void cpy(uintV_t buffer_in[SIZE], uintV_t buffer_out[SIZE]) {
cpy:
    for (int i = 0; i < SIZE; i++) {
        buffer_out[i] = buffer_in[i];
    }
}

void write(uintV_t buffer_out[SIZE], uintV_t* out) {
write:
    for (int i = 0; i < SIZE; i++) {
        out[i] = buffer_out[i];
    }
}

extern "C" {
void copy(uintV_t* in, uintV_t* out, const int t_size) {
#pragma HLS INTERFACE m_axi port = in offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = out offset = slave bundle = gmem
#pragma HLS INTERFACE s_axilite port = in
#pragma HLS INTERFACE s_axilite port = out
#pragma HLS INTERFACE s_axilite port = t_size
#pragma HLS INTERFACE s_axilite port = return

    uintV_t buffer_in[SIZE];
    uintV_t buffer_out[SIZE];

    int n_loop = t_size / (16 * SIZE);
    int offset = 0;

#pragma HLS dataflow
    for (int i = 0; i < n_loop; i++, offset += SIZE) {
        read(in + offset, buffer_in);
        cpy(buffer_in, buffer_out);
        write(buffer_out, out + offset);
    }
}
}
