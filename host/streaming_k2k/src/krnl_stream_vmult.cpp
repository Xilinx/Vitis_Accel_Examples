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
/*
In this example, we will demonstrate how to use Xilinx Streaming APIs.
            +----------+                                      +----------+
            |          |>>>>>>>>>>>> Input Stream 1 >>>>>>>>>>|          |
            |          |                                      |   VADD   |
            |   HOST   |>>>>>>>>>>>> Input Stream 2 >>>>>>>>>>|  Kernel  |
            |          |                                      +----------+
            |          |                                           |#|
            |          | |#|<----------- Internal Stream (Kernel to Kernel)
            |          |                                      +----------+
            |          |>>>>>>>>>>> Input Stream 3 >>>>>>>>>>>|          |
            |          |                                      |  VMULT   |
            |          |<<<<<<<<<<< Output Stream <<<<<<<<<<<<|  Kernel  |
            +----------+                                      +----------+
*/
#include "ap_axi_sdata.h"
#include "ap_int.h"
#include "hls_stream.h"

#define DWIDTH 32

typedef qdma_axis<DWIDTH, 0, 0, 0> pkt;
typedef ap_axiu<DWIDTH, 0, 0, 0> trans_pkt;

extern "C" {
void krnl_stream_vmult(hls::stream<pkt> &a, hls::stream<trans_pkt> &b,
                       hls::stream<pkt> &output) {
  bool eos = false;
vmult:
  do {
#pragma HLS PIPELINE II = 1
    // Reading a and b streaming into packets
    pkt t1 = a.read();
    trans_pkt t2 = b.read();

    // Packet for output
    pkt t_out;

    // Reading data from input packet
    ap_uint<DWIDTH> in1 = t1.get_data();
    ap_uint<DWIDTH> in2 = t2.data;

    // Vmult operation
    ap_uint<DWIDTH> tmpOut = in1 * in2;

    // Setting data and configuration to output packet
    t_out.set_data(tmpOut);
    t_out.set_last(t1.get_last());
    t_out.set_keep(-1); // Enabling all bytes

    // Writing packet to output stream
    output.write(t_out);

    if (t1.get_last() || t2.last) {
      eos = true;
    }
  } while (eos == false);
}
}
