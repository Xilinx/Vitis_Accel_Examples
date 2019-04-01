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

// This function represents an HLS kernel. The kernel will be called from
// host application using the xcl_run_kernels call. The pointers in kernel
// parameters represents cl_mem objects on the FPGA DDR memory.

#define BUFFER_SIZE 256
#define DATA_SIZE 1024

//TRIPCOUNT indentifier
const unsigned int c_len = DATA_SIZE/BUFFER_SIZE;
const unsigned int c_size = BUFFER_SIZE;

extern "C" {
void vector_add(int* c,
                int* a,
                int* b,
                const int n_elements)
{
#pragma HLS INTERFACE m_axi port=c offset=slave bundle=gmem
#pragma HLS INTERFACE m_axi port=a offset=slave bundle=gmem
#pragma HLS INTERFACE m_axi port=b offset=slave bundle=gmem

#pragma HLS INTERFACE s_axilite port=c bundle=control
#pragma HLS INTERFACE s_axilite port=a bundle=control
#pragma HLS INTERFACE s_axilite port=b bundle=control
#pragma HLS INTERFACE s_axilite port=n_elements bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

    int arrayA[BUFFER_SIZE];
    int arrayB[BUFFER_SIZE];
    for (int i = 0 ; i < n_elements ; i += BUFFER_SIZE) {
    #pragma HLS LOOP_TRIPCOUNT min=c_len max=c_len
        int size = BUFFER_SIZE;
        if (i + size > n_elements) size = n_elements - i;
        readA: for (int j = 0 ; j < size ; j++) {
        #pragma HLS PIPELINE II=1
        #pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
                arrayA[j] = a[i+j]; 
        }

        readB: for (int j = 0 ; j < size ; j++) {
        #pragma HLS PIPELINE II=1
        #pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
                arrayB[j] = b[i+j]; 
        }

        vadd_writeC: for (int j = 0 ; j < size ; j++) {
        #pragma HLS PIPELINE II=1
        #pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
                c[i+j] = arrayA[j] + arrayB[j]; 
        }
    }
}
}
