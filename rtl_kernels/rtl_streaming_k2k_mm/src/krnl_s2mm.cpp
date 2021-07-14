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
/* This is a stream to memory mapped data mover kernel which takes input from a
stream and writes data
to global memory via memory mapped interface */

#include "ap_axi_sdata.h"
#include "ap_int.h"
#include "hls_stream.h"

#define DWIDTH 512

typedef ap_axiu<DWIDTH, 0, 0, 0> pkt;

extern "C" {
void krnl_s2mm(ap_uint<512>* out,    // Write only memory mapped
               hls::stream<pkt>& in, // Internal Stream
               int size              // Size in integer
               ) {
#pragma HLS INTERFACE m_axi port = out offset = slave bundle = gmem
#pragma HLS INTERFACE axis port = in
#pragma HLS INTERFACE s_axilite port = out
#pragma HLS INTERFACE s_axilite port = size
#pragma HLS INTERFACE s_axilite port = return

data_mover:

    int i = 0;
    pkt v;
    // Auto-pipeline is going to apply pipeline to this loop
    for (int i = 0; i < (size * 32 / 512); i++) {
        v = in.read();
        out[i] = v.data;
    }
}
}
