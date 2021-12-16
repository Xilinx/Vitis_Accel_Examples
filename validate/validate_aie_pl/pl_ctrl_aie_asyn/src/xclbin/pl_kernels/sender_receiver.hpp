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

#ifndef _SENDER_RECEIVER_HPP_
#define _SENDER_RECEIVER_HPP_
#include "ap_axi_sdata.h"
#include "ap_int.h"
#include "hls_stream.h"
#include "xf_plctrl/enums.hpp"

static void mm2s(ap_uint<32>* mem, hls::stream<ap_axiu<32, 0, 0, 0> >& s, int size) {
    for (int i = 0; i < size; i++) {
#pragma HLS PIPELINE II = 1
        ap_axiu<32, 0, 0, 0> x;
        x.data = mem[i];
        x.keep = -1;
        if (i == size - 1)
            x.last = 1;
        else
            x.last = 0;
        s.write(x);
    }
}

static void s2mm(ap_uint<32>* mem, hls::stream<ap_axiu<32, 0, 0, 0> >& s, int size) {
    for (int i = 0; i < size; i++) {
#pragma HLS PIPELINE II = 1
        ap_axiu<32, 0, 0, 0> x = s.read();
        mem[i] = x.data;
    }
}
static void send_receive_df(ap_uint<32>* mem_in,
                            ap_uint<32>* mem_out,
                            hls::stream<ap_axiu<32, 0, 0, 0> >& s_in,
                            hls::stream<ap_axiu<32, 0, 0, 0> >& s_out,
                            int size) {
#pragma HLS dataflow
    mm2s(mem_in, s_out, size);
    s2mm(mem_out, s_in, size);
}

extern "C" {

void sender_receiver(int loop_count,
                     ap_uint<32>* mem_in,
                     ap_uint<32>* mem_out,
                     hls::stream<ap_axiu<32, 0, 0, 0> >& s_in,
                     hls::stream<ap_axiu<32, 0, 0, 0> >& s_out,
                     hls::stream<ap_axiu<32, 0, 0, 0> >& sync_in,
                     hls::stream<ap_axiu<32, 0, 0, 0> >& sync_out);
}

#endif
