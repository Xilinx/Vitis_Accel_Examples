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

#include "vpp_acc.hpp"

class xfilter : public VPP_ACC<xfilter, 1> {
    ZERO_COPY(in);
    ZERO_COPY(out);
    ZERO_COPY(outSz);
    SYS_PORT(in, DDR[0]);
    SYS_PORT(out, DDR[1]);
    SYS_PORT(outSz, DDR[2]);

    SYS_PORT_PFM(u50, in, HBM[0]);
    SYS_PORT_PFM(u50, out, HBM[1]);
    SYS_PORT_PFM(u50, outSz, HBM[2]);

    SYS_PORT_PFM(u280, in, HBM[0]);
    SYS_PORT_PFM(u280, out, HBM[1]);
    SYS_PORT_PFM(u280, outSz, HBM[2]);

    SYS_PORT_PFM(u55, in, HBM[0]);
    SYS_PORT_PFM(u55, out, HBM[1]);
    SYS_PORT_PFM(u55, outSz, HBM[2]);

   public:
    static void compute(int chunks, int chunkSz, int* in, int* out, int* outSz);
    static void hls_top(int chunks, int chunkSz, int* in, int* out, int* outSz);
};
