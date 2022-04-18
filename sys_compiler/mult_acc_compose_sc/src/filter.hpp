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
#include PFM_CONFIG_H(XILINX_SC_PFM_CONFIG)

class xfilter0 : public VPP_ACC<xfilter0, 1> {
    ZERO_COPY(in);
    ZERO_COPY(out);
    SYS_PORT(in, PORT_MAP1);
    SYS_PORT(out, PORT_MAP2);

   public:
    static void compute(int* in, int inSz, int* out, int* outSz);
    static void hls_top(int* in, int inSz, int* out, int* outSz);
};

class xfilter1 : public VPP_ACC<xfilter1, 1> {
    ZERO_COPY(in);
    ZERO_COPY(out);
    SYS_PORT(in, PORT_MAP2);
    SYS_PORT(out, PORT_MAP3);

   public:
    static void compute(int* in, int inSz, int* out, int* outSz);
    static void hls_top(int* in, int inSz, int* out, int* outSz);
};
