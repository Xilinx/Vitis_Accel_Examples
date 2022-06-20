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
#include "stream_krnl.hpp"

void fsk_acc::ldst(int* A, int* X, int sz, hls::stream<AXI>& AS, hls::stream<AXI>& XS) {
    for (int i = 0; i < sz; i++) {
        AXI ld;
        ld.data = A[i];
        ld.last = (i == sz - 1);
        AS.write(ld);
    }

    AXI st{0};
    for (int i = 0; i < sz; i++) {
        st = XS.read();
        X[i] = st.data;
    }
}

void fsk_acc::incr(hls::stream<AXI>& AS, hls::stream<AXI>& XS) {
    AXI pkt = AS.read();
    pkt.data++;
    XS.write(pkt);
}
