/**********
Copyright (c) 2019, Xilinx, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/
#include "ap_axi_sdata.h"
#include "ap_int.h"
#include "hls_stream.h"
#include "krnl_mmult.hpp"
extern "C" {
void krnl_simple_mmult(int *a, int *b, int *c, int *d, int *output, int dim) {

   #pragma HLS INTERFACE m_axi port = a offset = slave bundle = gmem0
   #pragma HLS INTERFACE m_axi port = b offset = slave bundle = gmem1
   #pragma HLS INTERFACE m_axi port = c offset = slave bundle = gmem2
   #pragma HLS INTERFACE m_axi port = d offset = slave bundle = gmem3
   #pragma HLS INTERFACE m_axi port = output offset = slave bundle = gmem3
   #pragma HLS INTERFACE s_axilite port = a bundle = control
   #pragma HLS INTERFACE s_axilite port = b bundle = control
   #pragma HLS INTERFACE s_axilite port = c bundle = control
   #pragma HLS INTERFACE s_axilite port = d bundle = control
   #pragma HLS INTERFACE s_axilite port = output bundle = control
   #pragma HLS INTERFACE s_axilite port = dim bundle = control
   #pragma HLS INTERFACE s_axilite port = return bundle = control

   #pragma HLS STABLE variable=a
   #pragma HLS STABLE variable=b
   #pragma HLS STABLE variable=c
   #pragma HLS STABLE variable=d
   #pragma HLS STABLE variable=output

    hls::stream<pkt> strm_a, strm_b, strm_c, strm_d;
    hls::stream<int> strm_ctrl_trans1, strm_ctrl_trans2, strm_ctrl_trans3,
        strm_ctrl_trans4, strm_ctrl_trans5;

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
