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

#include "filter.hpp"

void xfilter0::compute(int* in, int inSz, int* out, int* outSz) {
    hls_top(in, inSz, out, outSz);
}

void xfilter0::hls_top(int* in, int inSz, int* out, int* outSz) {
    int cnt = 0;
    for (int i = 0; i < inSz; i++) {
        if (in[i] != 0) {
            out[cnt++] = in[i];
        }
    }
    *outSz = cnt;
    while (cnt < inSz) {
        out[cnt++] = -2; // junk data -- should not be copied back to host
    }
}

void xfilter1::compute(int* in, int inSz, int* out, int* outSz) {
    hls_top(in, inSz, out, outSz);
}

void xfilter1::hls_top(int* in, int inSz, int* out, int* outSz) {
    int cnt = 0;
    for (int i = 0; i < inSz; i++) {
        if (in[i] != 1) {
            out[cnt++] = in[i];
        }
    }
    *outSz = cnt;
    while (cnt < inSz) {
        out[cnt++] = -2; // junk data -- should not be copied back to host
    }
}
