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

extern "C" {
void increment(hls::stream<qdma_axis<32, 0, 0, 0> >& input,
               hls::stream<qdma_axis<32, 0, 0, 0> >& output,
               int foo,
               int& beyond_foo) {
#pragma HLS interface axis port = input
#pragma HLS interface axis port = output
#pragma HLS interface s_axilite port = foo bundle = control
#pragma HLS interface ap_hs port = foo
#pragma HLS interface s_axilite port = beyond_foo bundle = control
#pragma HLS interface ap_ctrl_none port = return

    int count = 0;
increment:
    while (1) {
        qdma_axis<32, 0, 0, 0> v = input.read();
        if (v.data > foo) count++;
        output.write(v);
        if (v.last) {
            break;
        }
    }
    beyond_foo = count;
}
}
