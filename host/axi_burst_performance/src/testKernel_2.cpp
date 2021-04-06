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

#include "ap_int.h"
#include "hls_stream.h"
#include <cstdint>
#include <string.h>

// Parameters of the AXI-MM ports of the kernel
const int bl = 16; // Burst length
const int ot = 4;  // Outstanding transactions

typedef ap_int<512> int512_t;

void writeBuffer_2(int512_t* mem, int64_t buf_size) {
    buf_size = (buf_size / 1024) * 1024; // Make HLS see that buf_size is a multiple of 1024

write_buffer_2:
    for (int64_t i = 0; i < buf_size / 64; i++) {
        mem[i] = i;
    }
}

void readBuffer_2(int512_t* mem, int64_t buf_size, int64_t& err) {
    int64_t tmp = 0;

    buf_size = (buf_size / 1024) * 1024; // Make HLS see that buf_size is a multiple of 1024

read_buffer_2:
    for (int64_t i = 0; i < buf_size / 64; i++) {
        tmp += (mem[i] != i) ? 1 : 0;
    }

    err = tmp;
}

void testKernelProc_2(int512_t* mem, int64_t buf_size, int direction, hls::stream<int64_t>& cmd) {
    if (direction == 0) {
        cmd.write(0); // Send a command to start the counter
        writeBuffer_2(mem, buf_size);
        cmd.write(0); // Send a command to stop the counter
    } else {
        int64_t err;
        cmd.write(0); // Send a command to start the counter
        readBuffer_2(mem, buf_size, err);
        cmd.write(err); // Send a command to stop the counter
    }
}

void perfCounterProc_2(hls::stream<int64_t>& cmd, int64_t* out, int direction) {
    int64_t val;
    // wait to receive a value to start counting
    int64_t cnt = cmd.read();
// keep counting until a value is available
count_2:
    while (cmd.read_nb(val) == false) {
        cnt++;
    }

    // write out kernel statistics to global memory
    int64_t tmp[4];
    tmp[0] = cnt;
    tmp[1] = val;
    tmp[2] = bl;
    tmp[3] = ot;
    memcpy(out, tmp, 4 * sizeof(int64_t));
}

extern "C" {
void testKernel_2(int64_t buf_size, int direction, int64_t* perf, int512_t* mem) {
#pragma HLS INTERFACE m_axi port = mem bundle = aximm0 num_write_outstanding = ot max_write_burst_length = \
    bl num_read_outstanding = ot max_read_burst_length = bl offset = slave

#pragma HLS DATAFLOW

    hls::stream<int64_t> cmd;

    testKernelProc_2(mem, buf_size, direction, cmd);
    perfCounterProc_2(cmd, perf, direction);
}
}
