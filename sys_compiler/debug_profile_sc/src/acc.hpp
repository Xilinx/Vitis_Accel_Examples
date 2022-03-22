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
#include "vpp_acc.hpp"    // ACC base class
#include "vpp_stream.hpp" // stream class definition

// user-defined platform specific design config file
#include "config.hpp"

// Accelerator class
class pipelined_cu : public VPP_ACC<pipelined_cu, NCU> {
    // data mover from global memory to Acc
    ACCESS_PATTERN(A, SEQUENTIAL);
    ACCESS_PATTERN(B, SEQUENTIAL);
    ACCESS_PATTERN(E, SEQUENTIAL);
    // specify dynamic payload size (M * M) for each compute() call
    DATA_COPY(A, A[M * M]);
    DATA_COPY(B, B[M * M]);
    DATA_COPY(E, E[M * M]);
    // global memory mapping defined in config.hpp
    SYS_PORT(A, PORT_MAP_A);
    SYS_PORT(B, PORT_MAP_B);
    SYS_PORT(E, PORT_MAP_E);

#include "profile.hpp" // for HW profiling

   public:
    // ACC compute unit interface with dynamic data size (M)
    static void compute(float* A, float* B, float* E, int M);

    // pipeline processing elements (PEs within each CU)
    static void mmult(float* A, float* B, hls::stream<float>& X, int M);
    static void incr_10(hls::stream<float>& X, hls::stream<float>& Y, int M);
    static void incr_20(hls::stream<float>& Y, float* E, int M);
};
