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

/*******************************************************************************
Description: 
    C Kernel Example of Matrix Multiplication to demonstrate without array partition. 
*******************************************************************************/
				      
//Includes 
#include <stdio.h>
#include <string.h>

#define MAX_SIZE 16

//TRIPCOUNT identifiers
const unsigned int c_dim = MAX_SIZE;

extern "C" {
    // Naive implementation of matrix multiplication 
    // In this implementation array partition is not done
    void matmul(int *in1, int *in2, int *out_r, int size) {
    #pragma HLS INTERFACE m_axi port=in1 offset=slave bundle=gmem
    #pragma HLS INTERFACE m_axi port=in2 offset=slave bundle=gmem
    #pragma HLS INTERFACE m_axi port=out_r offset=slave bundle=gmem
    #pragma HLS INTERFACE s_axilite port=in1 bundle=control
    #pragma HLS INTERFACE s_axilite port=in2 bundle=control
    #pragma HLS INTERFACE s_axilite port=out_r bundle=control
    #pragma HLS INTERFACE s_axilite port=size bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control

        // Local buffers to hold input data    
        int A[MAX_SIZE][MAX_SIZE];
        int B[MAX_SIZE][MAX_SIZE];
        int C[MAX_SIZE][MAX_SIZE];
        int temp_sum[MAX_SIZE];

        // Read data from global memory and write into local buffer for in1
        readA:
        for (int itr = 0, i = 0, j = 0; itr < size * size; itr++, j++) {
        #pragma HLS LOOP_TRIPCOUNT min=c_dim*c_dim max=c_dim*c_dim
        #pragma HLS PIPELINE II=1
            if (j == size) { j = 0; i++; }
            A[i][j] = in1[itr];
        }

         // Read data from global memory and write into local buffer for in2
        readB:
        for (int itr = 0, i = 0, j = 0; itr < size * size; itr++, j++) {
        #pragma HLS LOOP_TRIPCOUNT min=c_dim*c_dim max=c_dim*c_dim
        #pragma HLS PIPELINE II=1
            if (j == size) { j = 0; i++; }
            B[i][j] = in2[itr];
        }
        
        // Calculate matrix multiplication using local data buffer based on input size, and write results into C
        nopart1: 
        for (int row = 0; row < size; row++) {
        #pragma HLS LOOP_TRIPCOUNT min=c_dim max=c_dim
            nopart2:
            for (int col = 0; col < size; col++) {
            #pragma HLS LOOP_TRIPCOUNT min=c_dim max=c_dim
            #pragma HLS PIPELINE
                nopart3:
                for (int j = 0; j < MAX_SIZE; j++) {
                #pragma HLS LOOP_TRIPCOUNT min=c_dim max=c_dim
                    int result = (col == 0) ? 0 : temp_sum[j];
                    result += A[row][col] * B[col][j];
                    temp_sum[j] = result;
                    if (col == size - 1) C[row][j] = result;
                }
            }
        }

        // Write results from local buffer to global memory for out
        writeC:
        for (int itr = 0, i = 0, j = 0; itr < size * size; itr++, j++) { 
        #pragma HLS LOOP_TRIPCOUNT min=c_dim*c_dim max=c_dim*c_dim
        #pragma HLS PIPELINE II=1
            if (j == size) { j = 0; i++; }
            out_r[itr] = C[i][j];
        }             
    }
}

