/**********
Copyright (c) 2020, Xilinx, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/
#include <ap_int.h>
#include <iostream>

auto constexpr DATAWIDTH = 512;
using TYPE = ap_uint<DATAWIDTH>;

// Tripcount identifiers
auto constexpr c_min_size = (1024 * 1024) / 64;
auto constexpr c_max_size = (1024 * 1024 * 1024) / 64;

extern "C" {
void bandwidth(TYPE *__restrict__ input0, TYPE *__restrict__ output0,
#if NDDR_BANKS == 3
               TYPE *__restrict__ output1,
#elif NDDR_BANKS > 3
               TYPE *__restrict__ input1, TYPE *__restrict__ output1,
#endif
               int64_t num_blocks) {

  for (int64_t blockindex = 0; blockindex < num_blocks; blockindex++) {
#pragma HLS LOOP_TRIPCOUNT min = c_min_size max = c_max_size
    TYPE temp0 = input0[blockindex];
    output0[blockindex] = temp0;
#if NDDR_BANKS == 3
    output1[blockindex] = temp0;
#elif NDDR_BANKS > 3
    TYPE temp1 = input1[blockindex];
    output1[blockindex] = temp1;
#endif
  }
}
}
