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

#include "sender_receiver.hpp"
#include "ap_axi_sdata.h"
#include "ap_int.h"
#include "hls_stream.h"
#include <stdio.h>

extern "C" {
void sender_receiver(int loop_count,
                     ap_uint<32>* mem_in,
                     ap_uint<32>* mem_out,
                     hls::stream<ap_axiu<32, 0, 0, 0> >& s_in,
                     hls::stream<ap_axiu<32, 0, 0, 0> >& s_out,
                     hls::stream<ap_axiu<32, 0, 0, 0> >& sync_in,
                     hls::stream<ap_axiu<32, 0, 0, 0> >& sync_out) {
#pragma HLS INTERFACE m_axi port = mem_in offset = slave bundle = gmem0
#pragma HLS INTERFACE m_axi port = mem_out offset = slave bundle = gmem1
#pragma HLS interface axis port = s_in
#pragma HLS interface axis port = s_out
#pragma HLS interface axis port = sync_in
#pragma HLS interface axis port = sync_out

#pragma HLS INTERFACE s_axilite port = mem_in bundle = control
#pragma HLS INTERFACE s_axilite port = mem_out bundle = control
#pragma HLS INTERFACE s_axilite port = loop_count bundle = control
#pragma HLS interface s_axilite port = return bundle = control
#ifndef __SYNTHESIS__
    printf("debug mem_in[1]: %d\n", (int)mem_in[1]);
#endif
    ap_uint<32> offt = 0;
    for (int i = 0; i < loop_count; ++i) {
        ap_axiu<32, 0, 0, 0> req = sync_in.read();
#ifndef __SYNTHESIS__
        assert(req.data == xf::plctrl::enums::SYNC_REQ);
#endif
        ap_axiu<32, 0, 0, 0> pld = sync_in.read();
        int size = pld.data;
        // int size = 32;
        send_receive_df(mem_in + offt, mem_out + offt, s_in, s_out, size);
// must wait sender and receiver
#ifdef __SYNTHESIS__
        _ssdm_op_Wait();
#endif
        offt += size;
        ap_axiu<32, 0, 0, 0> ack;
        ack.data = xf::plctrl::enums::SYNC_ACK;
        ack.keep = -1;
        ack.last = 0;
        sync_out.write(ack);
    }
}
}
