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

#include <ap_int.h>
#include <hls_stream.h>
#include <ap_axi_sdata.h>

const int NUM=2;
const int STREAM_WIDTH=32;

extern "C" {

void mm2s(ap_int<32>* mem, hls::stream<qdma_axis<32, 0, 0, 0> >& s0, hls::stream<qdma_axis<32, 0, 0, 0> >& s1, int size) {
data_mover:
    for (int i = 0; i < size; i++) {
        ap_int<STREAM_WIDTH*NUM> data=mem[i];
        qdma_axis<32, 0, 0, 0> x[NUM];
        for(int j=0;j<NUM;j++){
        x[j].data = data(STREAM_WIDTH*j+STREAM_WIDTH-1,STREAM_WIDTH*j);
        x[j].keep = -1;
        x[j].last = 0;
}        
        s0.write(x[0]);
        s1.write(x[1]);
    }
}
}
