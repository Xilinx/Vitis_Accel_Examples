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

//------------------------------------------------------------------------------
//
// kernel:  vadd
//
// Purpose: Demonstrate Vector Add in C++ HLS
//

#define LENGTH 256
#define BUFFER_SIZE 256

// Tripcount identifiers
const int c_size = BUFFER_SIZE;
const int c_len = LENGTH / BUFFER_SIZE;

extern "C" {
void krnl_vadd(int *a, int *e, const int length_r) {
#pragma HLS INTERFACE m_axi port = a offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = e offset = slave bundle = gmem
#pragma HLS INTERFACE s_axilite port = a bundle = control
#pragma HLS INTERFACE s_axilite port = e bundle = control
#pragma HLS INTERFACE s_axilite port = length_r bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

    int arrayA[BUFFER_SIZE];
    int arrayB[BUFFER_SIZE];
    int arrayC[BUFFER_SIZE];
    int arrayD[BUFFER_SIZE];

    for (int i = 0; i < length_r; i += BUFFER_SIZE) {
       #pragma HLS LOOP_TRIPCOUNT min=c_len max=c_len
        int size = BUFFER_SIZE;
        if (i + size > length_r)
            size = length_r - i;

    readA:
        for (int j = 0; j < 4 * size; j++) {
           #pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
           #pragma HLS PIPELINE II=1
            int tmpValue = a[i + j];
            switch (j % 4) {
            case 0:
                arrayA[j / 4] = tmpValue;
                break;
            case 1:
                arrayB[j / 4] = tmpValue;
                break;
            case 2:
                arrayC[j / 4] = tmpValue;
                break;
            case 3:
                arrayD[j / 4] = tmpValue;
                break;
            }
        }

    vadd_writeC:
        for (int j = 0; j < size; j++) {
           #pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
           #pragma HLS PIPELINE II=1
            e[j] = arrayA[j] + arrayB[j] + arrayC[j] + arrayD[j];
        }
    }
}
}
