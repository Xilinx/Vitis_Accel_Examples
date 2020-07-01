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

#define BUFFER_SIZE 1024
#define DATA_SIZE 4096

// TRIPCOUNT identifier
const unsigned int c_len = DATA_SIZE / BUFFER_SIZE;
const unsigned int c_size = BUFFER_SIZE;

extern "C" {
void vadd(const unsigned int *in1, // Read-Only Vector 1
          const unsigned int *in2, // Read-Only Vector 2
          unsigned int *out_r,     // Output Result
          int size                 // Size in integer
          ) {

  unsigned int v1_buffer[BUFFER_SIZE];   // Local memory to store vector1
  unsigned int v2_buffer[BUFFER_SIZE];   // Local memory to store vector2
  unsigned int vout_buffer[BUFFER_SIZE]; // Local Memory to store result

  // Using the BIND_OP pragma the user can specify the operator, implementation and latency
  #pragma HLS BIND_OP variable=v1_buffer op=mul  impl=DSP latency=2
  #pragma HLS BIND_OP variable=v2_buffer op=mul  impl=DSP latency=2
  #pragma HLS BIND_OP variable=vout_buffer op=add  impl=DSP 
  // Using the BIND STORAGE the used can choose the type, resource and latency
  #pragma HLS BIND_STORAGE variable=v1_buffer type=RAM_1P impl=BRAM latency=2
  #pragma HLS BIND_STORAGE variable=v2_buffer type=RAM_1P impl=LUTRAM latency=2
  #pragma HLS BIND_STORAGE variable=vout_buffer type=RAM_1P impl=URAM

  // Per iteration of this loop perform BUFFER_SIZE vector addition
  for (int i = 0; i < size; i += BUFFER_SIZE) {
#pragma HLS LOOP_TRIPCOUNT min = c_len max = c_len
    int chunk_size = BUFFER_SIZE;
    // boundary checks
    if ((i + BUFFER_SIZE) > size)
      chunk_size = size - i;

// Auto-pipeline is going to apply pipeline to these loops
  read1:
    for (int j = 0; j < chunk_size; j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
      v1_buffer[j] = in1[i + j] * in1[i + j];
    }

  read2:
    for (int j = 0; j < chunk_size; j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
      v2_buffer[j] = in2[i + j] * in2[i + j];
    }

  vadd:
    for (int j = 0; j < chunk_size; j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
      // perform vector addition
      vout_buffer[j] = v1_buffer[j] + v2_buffer[j];
    }

  // burst write the result
  write:
    for (int j = 0; j < chunk_size; j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
      out_r[i + j] = vout_buffer[j];
    }
  }
}
}
