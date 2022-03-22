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

void pipelined_cu::mmult(float* A, float* B, hls::stream<float>& X, int M) {
    float A_tmp[MaxM][MaxM], B_tmp[MaxM][MaxM];
#pragma HLS array_partition variable = A_tmp block factor = APF dim = 2
#pragma HLS array_partition variable = B_tmp block factor = APF dim = 1

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < M; j++) {
#pragma HLS pipeline
            A_tmp[i][j] = A[i * M + j];
            B_tmp[i][j] = B[i * M + j];
        }
    }

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < M; j++) {
#pragma HLS pipeline
            float result = 0;
            for (int k = 0; k < M; k++) {
#pragma HLS unroll factor = MaxM
                result += A_tmp[i][k] * B_tmp[k][j];
            }
            X << result;
        }
    }
}

void pipelined_cu::incr_10(hls::stream<float>& X, hls::stream<float>& Y, int M) {
    for (int i = 0; i < (M * M); i++) {
#pragma HLS PIPELINE
        Y << X.read() + 10;
    }
}

void pipelined_cu::incr_20(hls::stream<float>& Y, float* E, int M) {
    for (int i = 0; i < (M * M); i++) {
#pragma HLS PIPELINE
        E[i] = Y.read() + 20;
    }
}
