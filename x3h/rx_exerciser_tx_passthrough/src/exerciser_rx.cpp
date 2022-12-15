/*
 * Copyright 2022 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "x3_specs.hpp"
#include "pkt_fwd_utils.hpp"
#include "pkt_fwd_rx.hpp"

void exerciser_rx(hls::stream<ap_axiu<X3_RX_DWIDTH, X3_RX_UWIDTH, 0, 0> >& rx_in,
                  hls::stream<ap_axiu<X3_RX_DWIDTH, X3_RX_UWIDTH, 0, 0> >& rx_out,
                  hls::stream<ap_axiu<16, 0, 0, 0> >& table_in,
                  hls::stream<ap_axiu<16, 0, 0, 0> >& udp_dest) {
#pragma HLS interface axis port = rx_in
#pragma HLS interface axis port = rx_out
#pragma HLS interface axis port = table_in
#pragma HLS interface axis port = udp_dest
#ifndef __LIBRARY_PKT_FWD_DEBUG__
// infinite loop on stream, no AP control
#pragma HLS interface ap_ctrl_none port = return
#endif
#pragma HLS dataflow

    hls::stream<ap_uint<X3_RX_DWIDTH + 1> > dup1Strm("dup1Strm");
#pragma HLS stream variable = dup1Strm depth = 64
#pragma HLS BIND_STORAGE variable = dup1Strm type = FIFO impl = SRL
    hls::stream<ap_uint<X3_RX_DWIDTH + 1> > dup2Strm("dup2Strm");
#pragma HLS stream variable = dup2Strm depth = 64
#pragma HLS BIND_STORAGE variable = dup2Strm type = FIFO impl = SRL
    hls::stream<ap_uint<X3_RX_DWIDTH + 1> > dup3Strm("dup3Strm");
#pragma HLS stream variable = dup3Strm depth = 64
#pragma HLS BIND_STORAGE variable = dup3Strm type = FIFO impl = SRL
    hls::stream<ap_uint<X3_RX_DWIDTH / 8> > keepStrm("keepStrm");
#pragma HLS stream variable = keepStrm depth = 64
#pragma HLS BIND_STORAGE variable = keepStrm type = FIFO impl = SRL
    hls::stream<ap_uint<20> > ctrlStrm("ctrlStrm");
#pragma HLS stream variable = ctrlStrm depth = 32
#pragma HLS BIND_STORAGE variable = ctrlStrm type = FIFO impl = SRL
    hls::stream<ap_uint<X3_RX_UWIDTH> > userStrm("userStrm");
#pragma HLS stream variable = userStrm depth = 64
#pragma HLS BIND_STORAGE variable = userStrm type = FIFO impl = SRL

    dupStrm<X3_RX_DWIDTH, X3_RX_UWIDTH>(rx_in, dup1Strm, dup2Strm, userStrm);
    analyzeHeader<X3_RX_DWIDTH>(dup1Strm, ctrlStrm, table_in, udp_dest);
    writePacket(dup2Strm, userStrm, ctrlStrm, rx_out);
}
