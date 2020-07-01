/**********
Copyright (c) 2020, Xilinx, Inc.
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
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/
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
