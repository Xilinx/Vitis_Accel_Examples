/**
* Copyright (C) 2020 Xilinx, Inc
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

#include "test_kernel_common.hpp"

extern "C" {
void test_kernel_maxi_512bit_3(int64_t buf_size, int direction, int64_t* perf, ap_int<512>* mem) {
#pragma HLS INTERFACE m_axi port = mem bundle = aximm0 num_write_outstanding = 4 max_write_burst_length = \
    32 num_read_outstanding = 4 max_read_burst_length = 32 offset = slave

#pragma HLS DATAFLOW

    hls::stream<int64_t> cmd;

    testKernelProc(mem, buf_size, direction, cmd, 64);
    perfCounterProc(cmd, perf, direction, 32, 4);
}
}
