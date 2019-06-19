/**********
Copyright (c) 2018, Xilinx, Inc.
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
In this example, we will demonstrate how to use Xilinx Streaming APIs for direct host to device transfer.
            +----------+                                      +----------+
            |          |>>>>>>>>>>>> Input Stream 1 >>>>>>>>>>|          |
            |          |                                      |   VADD   |
            |   HOST   |---(OpenCL Buffers)----(Device DDR)---|  Kernel  |
            |          |                                      | (DEVICE) | 
            |          |<<<<<<<<<<<< Output Stream <<<<<<<<<<<|          |
            +----------+                                      +----------+
*/
#include "ap_axi_sdata.h"
#include "ap_int.h"
#include "hls_stream.h"

#define DWIDTH 32

typedef qdma_axis<DWIDTH, 0, 0, 0> pkt;

extern "C" {
void krnl_stream_vadd(hls::stream<pkt> &out, // Write-Only output Stream
                      hls::stream<pkt> &in1, // Input Stream 1
                      int *in2,              // Memory mapped Input Vector 2
                      int size               // Size in integer
) {
#pragma HLS INTERFACE m_axi port = in2 offset = slave bundle = gmem
#pragma HLS INTERFACE axis port = out
#pragma HLS INTERFACE axis port = in1
#pragma HLS INTERFACE s_axilite port = in2 bundle = control
#pragma HLS INTERFACE s_axilite port = size bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

vadd:
    for (int i = 0; i < size; i++) {
       #pragma HLS PIPELINE II=1
        // Local Packets
        pkt in_buf, out_buf;

        // Reading input stream data into packets
        in_buf = in1.read();

        // vadd operation
        int res = in_buf.get_data() + in2[i];

        // Setting data and configuration to output packet
        out_buf.set_data(res);
        out_buf.set_keep(-1);
        out_buf.set_last(in_buf.get_last());

        // Writing packets to output streams
        out.write(out_buf);
    }
}
}