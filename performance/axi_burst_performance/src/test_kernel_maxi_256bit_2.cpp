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

#include "test_kernel_common.hpp"

extern "C" {
void test_kernel_maxi_256bit_2(int64_t buf_size, int direction, int64_t* perf, ap_int<256>* mem) {
#pragma HLS INTERFACE m_axi port = mem bundle = aximm0 num_write_outstanding = 4 max_write_burst_length = \
    16 num_read_outstanding = 4 max_read_burst_length = 16 offset = slave

#pragma HLS DATAFLOW

    hls::stream<int64_t> cmd;

    testKernelProc(mem, buf_size, direction, cmd, 32);
    perfCounterProc(cmd, perf, direction, 16, 4);
}
}
