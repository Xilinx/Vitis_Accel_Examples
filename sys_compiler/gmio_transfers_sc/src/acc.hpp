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

struct acc_seq : VPP_ACC<acc_seq, 2> {
    ACCESS_PATTERN(A, SEQUENTIAL);
    ACCESS_PATTERN(X, SEQUENTIAL);
    DATA_COPY(A, A[sz]);
    DATA_COPY(X, X[sz]);
    SYS_PORT(A, DDR[0]);
    SYS_PORT(X, DDR[0]);
    SYS_PORT_PFM(u50, A, HBM[0]);
    SYS_PORT_PFM(u50, X, HBM[0]);
    SYS_PORT_PFM(u25, A, DDR[0]);
    SYS_PORT_PFM(u25, X, DDR[0]);
    SYS_PORT_PFM(u55, A, HBM[0]);
    SYS_PORT_PFM(u55, X, HBM[0]);

    SYS_PORT_PFM(zcu102, A, HP0);
    SYS_PORT_PFM(zcu102, X, HP0);

    static void compute(float* A, float* X, int sz);
    static void hls_kernel(float* A, float* X, int sz);
};

struct acc_rnd : VPP_ACC<acc_rnd, 2> {
    ACCESS_PATTERN(A, RANDOM);
    ACCESS_PATTERN(X, RANDOM);
    SYS_PORT(A, DDR[1]);
    SYS_PORT(X, DDR[1]);
    SYS_PORT_PFM(u50, A, HBM[1]);
    SYS_PORT_PFM(u50, X, HBM[1]);
    SYS_PORT_PFM(u55, A, HBM[1]);
    SYS_PORT_PFM(u55, X, HBM[1]);
    static void compute(float A[100], float X[100], int sz);
    static void hls_kernel(float A[100], float X[100], int sz);
};

struct acc_zc : VPP_ACC<acc_zc, 2> {
    ZERO_COPY(A, A[sz]);
    ZERO_COPY(X, X[sz]);
    SYS_PORT(A, DDR[2]);
    SYS_PORT(X, DDR[2]);
    SYS_PORT_PFM(u50, A, HBM[2]);
    SYS_PORT_PFM(u50, X, HBM[2]);
    SYS_PORT_PFM(u25, A, DDR[1]);
    SYS_PORT_PFM(u25, X, DDR[1]);
    SYS_PORT_PFM(u55, A, HBM[2]);
    SYS_PORT_PFM(u55, X, HBM[2]);

    SYS_PORT_PFM(zcu102, A, HP1);
    SYS_PORT_PFM(zcu102, X, HP1);

    static void compute(float* A, float* X, int sz);
    static void hls_kernel(float* A, float* X, int sz);
};
