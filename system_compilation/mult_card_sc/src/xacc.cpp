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
#include "xacc.hpp"

void xacc::compute(char* buf, int sz, int* cnt) {
    hls_top(buf, sz, cnt);
}

void xacc::hls_top(char* buf, int sz, int* cnt) {
    int lcl[26];
    for (int i = 0; i < 26; i++) {
        lcl[i] = 0;
    }
    for (int i = 0; i < sz; i++) {
        char c = buf[i];
        if (c >= 'a' && c <= 'z') c += 'A' - 'a';
        if (c >= 'A' && c <= 'Z') lcl[c - 'A']++;
    }
    for (int i = 0; i < 26; i++) {
        cnt[i] = lcl[i];
    }
}
