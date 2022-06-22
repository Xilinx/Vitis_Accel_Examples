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
#pragma once
#include "vpp_acc.hpp"

#define NCU 1

class xacc : public VPP_ACC<xacc, NCU> {
    ZERO_COPY(buf);
    ZERO_COPY(cnt);
    SYS_PORT(buf, HBM[0]);
    SYS_PORT(cnt, HBM[0]);

    SYS_PORT_PFM(u200, buf, DDR[0]);
    SYS_PORT_PFM(u200, cnt, DDR[1]);

    SYS_PORT_PFM(u250, buf, DDR[0]);
    SYS_PORT_PFM(u250, cnt, DDR[1]);

   public:
    static void compute(char* buf, int sz, int* cnt);
    static void hls_top(char* buf, int sz, int* cnt);
};
