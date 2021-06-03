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
void increment(hls::stream<ap_axiu<32, 0, 0, 0> >& input, hls::stream<ap_axiu<32, 0, 0, 0> >& output) {
// For free running kernel, user needs to specify ap_ctrl_none for return port.
// This will create the kernel without AXI lite interface. Kernel will always be
// in running states.
#pragma HLS interface ap_ctrl_none port = return

increment:
    while (1) {
        ap_axiu<32, 0, 0, 0> v = input.read();
        v.data = v.data + 1;
        output.write(v);
        if (v.last) {
            break;
        }
    }
}
}
