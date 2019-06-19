/**********
Copyright (c) 2018, Xilinx, Inc.
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
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/

// Tripcount identifiers
__constant int c_min_size = (1024*1024)/64;
__constant int c_max_size = (1024*1024*1024)/64;

kernel __attribute__ ((reqd_work_group_size(1,1,1)))
void bandwidth(__global uint16  * __restrict input0,
               __global uint16  * __restrict output0,
#if NDDR_BANKS == 3
               __global uint16  * __restrict output1,               
#elif NDDR_BANKS > 3       
               __global uint16  * __restrict input1,
               __global uint16  * __restrict output1,
#endif
               ulong num_blocks)
{
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_min_size, c_max_size)))
    for (ulong blockindex = 0; blockindex < num_blocks; blockindex++) {
        uint16 temp0 = input0[blockindex];
        output0[blockindex] = temp0;
#if NDDR_BANKS == 3
        output1[blockindex] = temp0;
#elif NDDR_BANKS > 3
        uint16 temp1 = input1[blockindex];
        output1[blockindex] = temp1;
#endif
    }
}
