/**********
Copyright (c) 2019, Xilinx, Inc.
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

/*******************************************************************************
Description:
    One cycle of data process includes: p2p data transfer (p), kernel copy (c)
    and XDMA (x). Conceptually, x, c and p happens consecutively. The pipeline
    is designed so that c(n) and x(n) will happen in parallel with p(n+1), in
    which n is the nth chunk of data.
*******************************************************************************/

#include <stdint.h>
#include <ap_int.h>
#define SIZE 1024

typedef ap_uint<512> uintV_t;

void read(uintV_t *in, uintV_t buffer_in[SIZE]){
read:
    for (int i = 0; i < SIZE; i++) {
        #pragma HLS PIPELINE II=1
		buffer_in[i] = in[i];
	}
}

void cpy(uintV_t buffer_in[SIZE], uintV_t buffer_out[SIZE]){
cpy:
    for (int i = 0; i < SIZE; i++) {
        #pragma HLS PIPELINE II=1
		buffer_out[i] = buffer_in[i];
	}    
}

void write(uintV_t buffer_out[SIZE], uintV_t *out){
write:
    for (int i = 0; i < SIZE; i++) {
        #pragma HLS PIPELINE II=1
		out[i] = buffer_out[i];
	}
}

extern "C" {
void copy(uintV_t *in,
          uintV_t *out, 
          const int t_size
) {
    #pragma HLS INTERFACE m_axi port = in offset = slave bundle = gmem
    #pragma HLS INTERFACE m_axi port = out offset = slave bundle = gmem
    #pragma HLS INTERFACE s_axilite port = in bundle = control
    #pragma HLS INTERFACE s_axilite port = out bundle = control
    #pragma HLS INTERFACE s_axilite port = t_size bundle = control
    #pragma HLS INTERFACE s_axilite port = return bundle = control

    uintV_t buffer_in[SIZE];
	uintV_t buffer_out[SIZE];

    int n_loop = t_size / (16 * SIZE);
	int offset = 0;

#pragma HLS dataflow
    for (int i = 0; i < n_loop; i++, offset += SIZE) {
		read(in + offset, buffer_in);
		cpy(buffer_in, buffer_out);
		write(buffer_out, out + offset);
	}
  }
    
}
