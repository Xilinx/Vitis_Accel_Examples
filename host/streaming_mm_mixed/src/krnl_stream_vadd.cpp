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
In this example, we will demonstrate how to use Xilinx Streaming APIs for direct
host to device transfer.
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
void krnl_stream_vadd(hls::stream<pkt>& out, // Write-Only output Stream
                      hls::stream<pkt>& in1, // Input Stream 1
                      int* in2,              // Memory mapped Input Vector 2
                      int size               // Size in integer
                      ) {
// Auto-pipeline is going to apply pipeline to this loop
vadd:
    for (int i = 0; i < size; i++) {
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
