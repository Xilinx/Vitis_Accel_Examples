/**
 * Copyright (C) 2022 Xilinx, Inc
 * Copyright (C) 2022-2023 Advanced Micro Devices, Inc.
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
#include "x3_specs.hpp"

typedef ap_axiu<X3_TX_DWIDTH, X3_TX_UWIDTH, 1, 1> trans_pkt_data;
typedef ap_axiu<X3_TX_META_RES_DWIDTH, 1, 1, 1> trans_pkt_meta_res;
typedef ap_axiu<X3_TX_PAUSE, 1, 1, 1> trans_pkt_pause;

void tx_packet_read(hls::stream<trans_pkt_data>& tx_in, hls::stream<trans_pkt_data>& tx_out) {
#pragma HLS INTERFACE ap_ctrl_none port = return
#pragma HLS PIPELINE
tx_packet_read:
    tx_out.write(tx_in.read());
}

void tx_packet_meta_res_read(hls::stream<trans_pkt_meta_res>& tx_meta_res_in,
                             hls::stream<trans_pkt_meta_res>& tx_meta_res_out) {
#pragma HLS INTERFACE ap_ctrl_none port = return
#pragma HLS PIPELINE
tx_packet_meta_res_read:
    tx_meta_res_out.write(tx_meta_res_in.read());
}

void tx_pause_read(hls::stream<trans_pkt_pause>& tx_pause_in, hls::stream<trans_pkt_pause>& tx_pause_out) {
#pragma HLS INTERFACE ap_ctrl_none port = return
#pragma HLS PIPELINE
tx_pause_read:
    tx_pause_out.write(tx_pause_in.read());
}

extern "C" {
void packet_passthrough_tx_v1(hls::stream<trans_pkt_data>& tx_in,
                              hls::stream<trans_pkt_data>& tx_out,
                              hls::stream<trans_pkt_meta_res>& tx_meta_res_in,
                              hls::stream<trans_pkt_meta_res>& tx_meta_res_out,
                              hls::stream<trans_pkt_pause>& tx_pause_in,
                              hls::stream<trans_pkt_pause>& tx_pause_out) {
#pragma HLS interface axis port = tx_in
#pragma HLS interface axis port = tx_out
#pragma HLS interface axis port = tx_meta_res_in
#pragma HLS interface axis port = tx_meta_res_out
#pragma HLS interface axis port = tx_pause_in
#pragma HLS interface axis port = tx_pause_out
#pragma HLS INTERFACE ap_ctrl_none port = return

#pragma HLS STREAM variable = tx_in depth = 16
#pragma HLS STREAM variable = tx_out depth = 16
#pragma HLS STREAM variable = tx_meta_res_in depth = 16
#pragma HLS STREAM variable = tx_meta_res_out depth = 16

#pragma HLS dataflow
    tx_packet_read(tx_in, tx_out);
    tx_packet_meta_res_read(tx_meta_res_in, tx_meta_res_out);
    tx_pause_read(tx_pause_in, tx_pause_out);
}
}
