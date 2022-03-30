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

void xfilter::compute(int chunks, int chunkSz, int* in, int* out, int* outSz) {
    hls_top(chunks, chunkSz, in, out, outSz);
}

void xfilter::hls_top(int chunks, int chunkSz, int* in, int* out, int* outSz) {
    for (int chunk = 0; chunk < chunks; chunk++) {
        int offset = chunk * chunkSz;
        int cnt = 0;
        for (int i = 0; i < chunkSz; i++) {
            if (in[offset + i] != 0) {
                out[offset + cnt++] = in[offset + i];
            }
        }
        outSz[chunk] = cnt;
        while (cnt < chunkSz) {
            out[offset + cnt++] = -2; // put in junk data to assert that it is not copied back to host
        }
    }
}
