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

/********************************************************************************************
 * Description:
 *
 * In this example, we will demonstrate emulation of Xilinx Device's IO with an external process.
 *
 *             +----------+                                               +----------+
 *             |  Traffic |                                               |          |
 *             | Generator|>>>>>>>>>>>> Input  IO of the device >>>>>>>>>>|          |
 *             |          |                                               |          |
 *             +----------+                                               |          |
 *                                                                        |  ADDER   |
 *                                                                        |  Kernel  |
 *             +----------+                                               |          |
 *             |  Traffic |                                               |          |
 *             | Receiver |<<<<<<<<<<<< Output IO of the device <<<<<<<<<<|          |
 *             |          |                                               |          |
 *             +----------+                                               +----------+
 *
 *  *****************************************************************************************/

#include "ap_axi_sdata.h"
#include "ap_int.h"
#include "hls_stream.h"

#define DWIDTH 32

typedef qdma_axis<DWIDTH, 0, 0, 0> pkt;

extern "C" {
void increment(hls::stream<pkt>& a, hls::stream<pkt>& output) {
    bool eos = false;
adder1:
    do {
#pragma HLS PIPELINE II = 1
        // Reading a stream into packets
        pkt t1 = a.read();

        // Packet for output
        pkt t_out;

        // Reading data from input packet and incrementing it by 1
        ap_uint<DWIDTH> in1 = t1.get_data() + 0x01;
        ap_uint<DWIDTH> tmpOut;

        // Setting data and configuration to output packet
        t_out.set_data(in1);
        t_out.set_last(t1.get_last());
        t_out.set_keep(-1); // Enabling all bytes

        // Writing packet to output stream
        output.write(t_out);

    } while (eos == false);
}
}
