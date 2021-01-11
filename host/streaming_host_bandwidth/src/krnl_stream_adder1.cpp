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
/*In this example, we will demonstrate how to use Xilinx Streaming APIs for
   direct host to device transfer.
            +----------+                                      +----------+
            |          |>>>>>>>>>>>> Input Stream 1 >>>>>>>>>>|          |
            |          |                                      |  ADDER   |
            |   HOST   |                                      |  Kernel  |
            |          |                                      |          |
            |          |<<<<<<<<<<<< Output Stream <<<<<<<<<<<|          |
            +----------+                                      +----------+
*/
#include "ap_axi_sdata.h"
#include "ap_int.h"
#include "hls_stream.h"

#define DWIDTH 512
#define DATATYPE_SIZE 32
#define VECTOR_SIZE (DWIDTH / DATATYPE_SIZE) // vector size is 16 (512/32 = 16)

typedef qdma_axis<DWIDTH, 0, 0, 0> pkt;

extern "C" {
void krnl_stream_adder1(hls::stream<pkt>& a, hls::stream<pkt>& output) {
    bool eos = false;
// Auto-pipeline is going to apply pipeline to this loop
adder1:
    do {
        // Reading a stream into packets
        pkt t1 = a.read();

        // Packet for output
        pkt t_out;

        // Reading data from input packet
        ap_uint<DWIDTH> in1 = t1.get_data();
        ap_uint<DWIDTH> tmpOut;

        // Parallel Adder
        for (int j = 0; j < VECTOR_SIZE; j++) {
#pragma HLS UNROLL
            ap_uint<DATATYPE_SIZE> temp_a = in1.range(DATATYPE_SIZE * (j + 1) - 1, j * DATATYPE_SIZE);

            // Increment operation
            ap_uint<DATATYPE_SIZE> temp_res = temp_a + 1;

            tmpOut.range(DATATYPE_SIZE * (j + 1) - 1, j * DATATYPE_SIZE) = temp_res;
        }

        // Setting data and configuration to output packet
        t_out.set_data(tmpOut);
        t_out.set_last(t1.get_last());
        t_out.set_keep(-1); // Enabling all bytes

        // Writing packet to output stream
        output.write(t_out);

        if (t1.get_last()) {
            eos = true;
        }
    } while (eos == false);
}
}
