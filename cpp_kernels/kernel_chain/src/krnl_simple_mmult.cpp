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
#include "ap_axi_sdata.h"
#include "ap_int.h"
#include "hls_stream.h"
#include "krnl_mmult.hpp"
extern "C" {
void krnl_simple_mmult(int* a, int* b, int* c, int* d, int* output, int dim) {
#pragma HLS INTERFACE m_axi port = a offset = slave bundle = gmem0
#pragma HLS INTERFACE m_axi port = b offset = slave bundle = gmem1
#pragma HLS INTERFACE m_axi port = c offset = slave bundle = gmem2
#pragma HLS INTERFACE m_axi port = d offset = slave bundle = gmem3
#pragma HLS INTERFACE m_axi port = output offset = slave bundle = gmem3
#pragma HLS INTERFACE s_axilite port = a
#pragma HLS INTERFACE s_axilite port = b
#pragma HLS INTERFACE s_axilite port = c
#pragma HLS INTERFACE s_axilite port = d
#pragma HLS INTERFACE s_axilite port = output
#pragma HLS INTERFACE s_axilite port = dim
#pragma HLS INTERFACE s_axilite port = return
#pragma HLS INTERFACE ap_ctrl_hs port = return

#pragma HLS STABLE variable = a
#pragma HLS STABLE variable = b
#pragma HLS STABLE variable = c
#pragma HLS STABLE variable = d
#pragma HLS STABLE variable = output

    hls::stream<pkt> strm_a, strm_b, strm_c, strm_d;
    hls::stream<int> strm_ctrl_trans1, strm_ctrl_trans2, strm_ctrl_trans3, strm_ctrl_trans4, strm_ctrl_trans5;

    int tmp = dim;
    strm_ctrl_trans1.write(tmp);

#pragma HLS STREAM variable = strm_ctrl_trans1 depth = 2
#pragma HLS STREAM variable = strm_ctrl_trans2 depth = 2
#pragma HLS STREAM variable = strm_ctrl_trans3 depth = 2
#pragma HLS STREAM variable = strm_ctrl_trans4 depth = 2
#pragma HLS STREAM variable = strm_ctrl_trans5 depth = 2

#pragma HLS STREAM variable = strm_a depth = 64
#pragma HLS STREAM variable = strm_b depth = 64
#pragma HLS STREAM variable = strm_c depth = 64
#pragma HLS STREAM variable = strm_d depth = 64

#pragma HLS DATAFLOW

    mm2s(a, strm_a, strm_ctrl_trans1, strm_ctrl_trans2);
    mmult(strm_a, b, strm_ctrl_trans2, strm_b, strm_ctrl_trans3);
    mmult(strm_b, c, strm_ctrl_trans3, strm_c, strm_ctrl_trans4);
    mmult(strm_c, d, strm_ctrl_trans4, strm_d, strm_ctrl_trans5);
    s2mm(strm_d, output, strm_ctrl_trans5);
}
}
