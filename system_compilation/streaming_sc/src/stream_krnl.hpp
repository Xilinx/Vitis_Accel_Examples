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
#include "vpp_stream.hpp"
#include "ap_axi_sdata.h"

#define DEPTH 32

using DT = int;
using AXI = hls::axis<DT, 0, 0, 0>;

class fsk_acc : public VPP_ACC<fsk_acc, 2> {
    ZERO_COPY(A, A[0]);
    ZERO_COPY(X, X[0]);

    SYS_PORT(A, DDR[0]);
    SYS_PORT(X, DDR[0]);

    SYS_PORT_PFM(u50, A, HBM[0]);
    SYS_PORT_PFM(u50, X, HBM[0]);

    SYS_PORT_PFM(u280, A, (HBM[0] : HBM[1]));
    SYS_PORT_PFM(u280, X, (HBM[0] : HBM[1]));

    SYS_PORT_PFM(u2_gen3x, A, bank0);
    SYS_PORT_PFM(u2_gen3x, X, bank0);

    SYS_PORT_PFM(u55, A, (HBM[0] : HBM[1]));
    SYS_PORT_PFM(u55, X, (HBM[0] : HBM[1]));

    FREE_RUNNING(incr);

   public:
    static void compute(int* A, int* X, int sz);

    static void ldst(int* A, int* X, int sz, hls::stream<AXI>& AS, hls::stream<AXI>& XS);

    static void incr(hls::stream<AXI>& AS, hls::stream<AXI>& XS);
};
