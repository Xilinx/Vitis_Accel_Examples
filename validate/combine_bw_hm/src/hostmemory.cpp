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
#include <ap_int.h>

auto constexpr DATAWIDTH = 512;
using TYPE = ap_uint<DATAWIDTH>;

extern "C" {
void hostmemory(TYPE* input, TYPE* output, unsigned int buf_size, unsigned int reps) {
    unsigned int num_blocks = (buf_size - 1) / 64 + 1;
read_write_outer:
    for (int repindex = 0; repindex < reps; repindex++) {
    read_write_inner:
        for (int blockindex = 0; blockindex < num_blocks; blockindex++) {
            TYPE temp = input[blockindex];
            output[blockindex] = temp;
        }
    }
}
}
