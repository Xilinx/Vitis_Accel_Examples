/**********
Copyright (c) 2019, Xilinx, Inc.
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
            |   HOST   |>>>>>>>>>>>> Input Stream 2 >>>>>>>>>>|  Kernel  |
            |          |                                      |          | 
            |          |<<<<<<<<<<<< Output Stream <<<<<<<<<<<|          |
            +----------+                                      +----------+
*/
#include "ap_axi_sdata.h"
#include "ap_int.h"
#include "hls_stream.h"

#define DWIDTH 32

typedef qdma_axis<DWIDTH, 0, 0, 0> pkt;

extern "C" {
void krnl_stream_vadd(hls::stream<pkt> &a,
                      hls::stream<pkt> &b,
                      hls::stream<pkt> &output) {
   #pragma HLS INTERFACE axis port=a
   #pragma HLS INTERFACE axis port=b
   #pragma HLS INTERFACE axis port=output
   #pragma HLS INTERFACE s_axilite port=return bundle=control

    bool eos = false;
vadd:
    do {
       #pragma HLS PIPELINE II=1
        // Reading a and b streaming into packets
        pkt t1 = a.read();
        pkt t2 = b.read();

        // Packet for output
        pkt t_out;

        // Reading data from input packet
        ap_uint<DWIDTH> in1 = t1.get_data();
        ap_uint<DWIDTH> in2 = t2.get_data();

        // Vadd operation
        ap_uint<DWIDTH> tmpOut = in1 + in2;

        // Setting data and configuration to output packet
        t_out.set_data(tmpOut);
        t_out.set_last(t1.get_last());
        t_out.set_keep(-1); // Enabling all bytes

        // Writing packet to output stream
        output.write(t_out);

        if (t1.get_last() || t2.get_last()) {
            eos = true;
        }
    } while (eos == false);
}
}