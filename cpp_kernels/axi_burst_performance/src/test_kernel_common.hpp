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

template <typename T>

void writeBuffer(T* mem, int64_t buf_size, int burst_size) {
    buf_size = (buf_size / 1024) * 1024; // Make HLS see that buf_size is a multiple of 1024

write_buffer:
    for (int64_t i = 0; i < buf_size / burst_size; i++) {
        mem[i] = i;
    }
}

template <typename T>

void readBuffer(T* mem, int64_t buf_size, int64_t& err, int burst_size) {
    int64_t tmp = 0;

    buf_size = (buf_size / 1024) * 1024; // Make HLS see that buf_size is a multiple of 1024

read_buffer:
    for (int64_t i = 0; i < buf_size / burst_size; i++) {
        tmp += (mem[i] != i) ? 1 : 0;
    }

    err = tmp;
}

// Template to avoid signature conflict in sw_emu
template <typename T, int DUMMY = 0>
void testKernelProc(T* mem, int64_t buf_size, int direction, hls::stream<int64_t>& cmd, int burst_size) {
    if (direction == 0) {
        cmd.write(0); // Send a command to start the counter
        writeBuffer(mem, buf_size, burst_size);
        cmd.write(0); // Send a command to stop the counter
    } else {
        int64_t err;
        cmd.write(0); // Send a command to start the counter
        readBuffer(mem, buf_size, err, burst_size);
        cmd.write(err); // Send a command to stop the counter
    }
}

// Template to avoid signature conflict in sw_emu
template <int DUMMY = 0>
void perfCounterProc(hls::stream<int64_t>& cmd, int64_t* out, int direction, int bl, int ot) {
    int64_t val;
    // wait to receive a value to start counting
    int64_t cnt = cmd.read();
// keep counting until a value is available
count:
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
