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

/* This is a data mover kernel which reads data from global memory(DDR) via
memory mapped interface
and writes to a stream interface to another kernel */

#include "ap_axi_sdata.h"
#include "ap_int.h"
#include "hls_stream.h"

#define DWIDTH 512

typedef ap_axiu<DWIDTH, 0, 0, 0> pkt;

extern "C" {
void krnl_mm2s(ap_uint<512>* in,      // Read-Only Vector 1
               hls::stream<pkt>& out, // Internal Stream
               int size               // Size in integer
               ) {
#pragma HLS INTERFACE m_axi port = in offset = slave bundle = gmem
#pragma HLS INTERFACE axis port = out
#pragma HLS INTERFACE s_axilite port = in
#pragma HLS INTERFACE s_axilite port = size
#pragma HLS INTERFACE s_axilite port = return

data_mover:

    int chunk_size = (DWIDTH / (sizeof(int) * 8));
    pkt v;

    int i = 0;
    // Auto-pipeline is going to apply pipeline to this loop
    for (i = 0; i < (size / chunk_size) - 1; i++) {
        ap_uint<512> res = in[i];
        v.data = res;
        v.last = 0;
        out.write(v);

        // set stream last signals when last data
    }
    ap_uint<512> res = in[i];
    v.data = res;
    v.last = 1;
    out.write(v);
}
}
