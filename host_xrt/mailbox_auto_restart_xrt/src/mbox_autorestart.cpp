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

extern "C" {
void mbox_autorestart(int in1, int in2, int& add, int& mult) {
#pragma HLS interface ap_ctrl_chain port = return
#pragma HLS INTERFACE s_axilite port = in1
#pragma HLS STABLE variable = in1
#pragma HLS INTERFACE s_axilite port = in2
#pragma HLS STABLE variable = in2
#pragma HLS INTERFACE s_axilite port = add
#pragma HLS STABLE variable = add
#pragma HLS INTERFACE s_axilite port = mult
#pragma HLS STABLE variable = mult

    add = in1 + in2;
    mult = in1 * in2;
}
}
