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

/*******************************************************************************
Description:
    This is a simple vector addition example using C++ HLS.

*******************************************************************************/

#define VDATA_SIZE 16

// TRIPCOUNT indentifier
const unsigned int c_dt_size = VDATA_SIZE;

typedef struct v_datatype { unsigned int data[VDATA_SIZE]; } v_dt;

extern "C" {
void krnl_vadd(const v_dt *in1,        // Read-Only Vector 1
               const v_dt *in2,        // Read-Only Vector 2
               v_dt *out_r,            // Output Result for Addition
               const unsigned int size // Size in integer
               ) {
#pragma HLS INTERFACE m_axi port = in1 offset = slave bundle = gmem0
#pragma HLS INTERFACE m_axi port = in2 offset = slave bundle = gmem1
#pragma HLS INTERFACE m_axi port = out_r offset = slave bundle = gmem0

#pragma HLS INTERFACE s_axilite port = in1
#pragma HLS INTERFACE s_axilite port = in2
#pragma HLS INTERFACE s_axilite port = out_r
#pragma HLS INTERFACE s_axilite port = size
#pragma HLS INTERFACE s_axilite port = return

  unsigned int vSize = ((size - 1) / VDATA_SIZE) + 1;

// Auto-pipeline is going to apply pipeline to this loop
vadd1:
  for (int i = 0; i < vSize; i++) {
  vadd2:
    for (int k = 0; k < VDATA_SIZE; k++) {
#pragma HLS LOOP_TRIPCOUNT min = c_dt_size max = c_dt_size
      out_r[i].data[k] = in1[i].data[k] + in2[i].data[k];
    }
  }
}
}
