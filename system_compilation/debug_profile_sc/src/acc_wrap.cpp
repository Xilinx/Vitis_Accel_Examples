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

// STREAM_DEPTH is defined in config.hpp
typedef vpp::stream<float, STREAM_DEPTH> InternalStream;

// Each CU has a pipeline of these processing elements (PEs)
void pipelined_cu::compute(float* A, float* B, float* E, int M) {
    static InternalStream X("str_X");
    static InternalStream Y("str_Y");

    // PE reads matrices A and B from memory, and write to stream out X
    mmult(A, B, X, M);

    // PE reads stream X, writes to stream Y
    incr_10(X, Y, M);

    // PE reads stream Y, writes to memory E
    incr_20(Y, E, M);
}
