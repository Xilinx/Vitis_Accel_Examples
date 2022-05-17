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

#include "acc.hpp"

void acc_hls_kernel(float* A, float* X, int sz) {
#pragma HLS INLINE
    for (int i = 0; i < sz; i++) {
        X[i] = A[i] + 0.0001 * i;
    }
}

void acc_seq::compute(float* A, float* X, int sz) {
    hls_kernel(A, X, sz);
}

void acc_seq::hls_kernel(float* A, float* X, int sz) {
    acc_hls_kernel(A, X, sz);
}

void acc_rnd::compute(float A[100], float X[100], int sz) {
    hls_kernel(A, X, sz);
}

void acc_rnd::hls_kernel(float A[100], float X[100], int sz) {
    acc_hls_kernel(A, X, sz);
}

void acc_zc::compute(float* A, float* X, int sz) {
    hls_kernel(A, X, sz);
}

void acc_zc::hls_kernel(float* A, float* X, int sz) {
    acc_hls_kernel(A, X, sz);
}
