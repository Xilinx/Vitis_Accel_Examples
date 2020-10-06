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
#include <ap_axi_sdata.h>
#include <ap_int.h>
#include <hls_stream.h>

#define DWIDTH 512
typedef qdma_axis<DWIDTH, 0, 0, 0> pkt;
extern "C" {
void increment(hls::stream<pkt> &input, hls::stream<pkt> &output) {
#pragma HLS interface axis port = input
#pragma HLS interface axis port = output

// For free running kernel, user needs to specify ap_ctrl_none for return port.
// This will create the kernel without AXI lite interface. Kernel will always be
// in running states.
#pragma HLS interface ap_ctrl_none port = return

increment1:
  while (true) {
    // Read the input data from the qdma
    pkt t = input.read();
    pkt t_out;
    ap_uint<DWIDTH> tmp_out;

    // Setup the output data
    ap_uint<DWIDTH> tmp = t.data;
    for (int i = 0; i < 16; i++) {
#pragma HLS UNROLL
      tmp_out.range((i + 1) * 32 - 1, i * 32) =
          tmp.range((i + 1) * 32 - 1, i * 32) + 1;
    }
    t_out.data = tmp_out;
    t_out.set_keep(-1);
    t_out.set_last(0);

    if (t.get_last()) {
      t_out.set_last(1);
    }
    output.write(t_out);
  }
}
}
