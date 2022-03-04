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
#include "common.h"
#include "vpp_acc.hpp"

class conv_acc : public VPP_ACC<conv_acc, /*NCU*/ 3> {
    ACCESS_PATTERN(src, SEQUENTIAL);
    ACCESS_PATTERN(coeffs, SEQUENTIAL);
    ACCESS_PATTERN(dst, SEQUENTIAL);
    // Data copy macros : specifies that size of data to be copied for kernel call in
    // case the kernel can process variable size data.
    DATA_COPY(src, src[width * height]);
    DATA_COPY(coeffs, coeffs[FILTER_V_SIZE * FILTER_H_SIZE]);
    DATA_COPY(dst, dst[width * height]);

    // Kernel DDR connections
    SYS_PORT(coeffs, DDR[0]);
    SYS_PORT(src, DDR[0]);
    SYS_PORT(dst, DDR[0]);

    SYS_PORT_PFM(u50, coeffs, HBM[0]);
    SYS_PORT_PFM(u50, src, HBM[0]);
    SYS_PORT_PFM(u50, dst, HBM[0]);

    SYS_PORT_PFM(u55, coeffs, HBM[0]);
    SYS_PORT_PFM(u55, src, HBM[0]);
    SYS_PORT_PFM(u55, dst, HBM[0]);

    SYS_PORT_PFM(u280, coeffs, HBM[0]);
    SYS_PORT_PFM(u280, src, HBM[0]);
    SYS_PORT_PFM(u280, dst, HBM[0]);

    SYS_PORT_PFM(u2_gen3x4, coeffs, bank0);
    SYS_PORT_PFM(u2_gen3x4, src, bank0);
    SYS_PORT_PFM(u2_gen3x4, dst, bank0);

   public:
    static void compute(char* coeffs,
                        float factor,
                        short bias,
                        unsigned short width,
                        unsigned short height,
                        unsigned char* src,
                        unsigned char* dst);
    static void krnl_conv(char* coeffs,
                          float factor,
                          short bias,
                          unsigned short width,
                          unsigned short height,
                          unsigned char* src,
                          unsigned char* dst);
};
