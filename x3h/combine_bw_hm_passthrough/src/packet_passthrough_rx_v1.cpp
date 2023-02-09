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
#include <unistd.h>
#include <stdint.h>

typedef ap_axiu<X3_RX_DWIDTH, X3_RX_UWIDTH, 1, 1> trans_pkt_data;
typedef ap_axiu<X3_RX_PAUSE, 1, 1, 1> trans_pkt_pause;

void rx_packet_read(hls::stream<trans_pkt_data>& rx_in, hls::stream<trans_pkt_data>& rx_out) {
#pragma HLS INTERFACE ap_ctrl_none port = return
#pragma HLS PIPELINE
rx_packet_read:
    rx_out.write(rx_in.read());
}

void rx_pause_read(hls::stream<trans_pkt_pause>& rx_pause_in, hls::stream<trans_pkt_pause>& rx_pause_out) {
#pragma HLS INTERFACE ap_ctrl_none port = return
#pragma HLS PIPELINE
rx_pause_read:
    rx_pause_out.write(rx_pause_in.read());
}

extern "C" {
void packet_passthrough_rx_v1(hls::stream<trans_pkt_data>& rx_in,
                              hls::stream<trans_pkt_data>& rx_out,
                              hls::stream<trans_pkt_pause>& rx_pause_in,
                              hls::stream<trans_pkt_pause>& rx_pause_out) {
#pragma HLS interface axis port = rx_in
#pragma HLS interface axis port = rx_out
#pragma HLS interface axis port = rx_pause_in
#pragma HLS interface axis port = rx_pause_out
#pragma HLS INTERFACE ap_ctrl_none port = return

#pragma HLS dataflow
    rx_packet_read(rx_in, rx_out);
  #ifndef SW_EMU_TEST
    rx_pause_read(rx_pause_in, rx_pause_out);
  #endif
}
}
