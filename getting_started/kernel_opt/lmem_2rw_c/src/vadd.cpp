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
    C Kernel to showcase 2 parallel read/write from  Local Memory 
    Description: This is vector addition to demonstrate how to utilized both
    ports of Local Memory. 
*******************************************************************************/

#include <string.h>

#define DATA_SIZE 4096
#define BUFFER_SIZE 1024

//TRIPCOUNT identifiers
const unsigned int c_chunk_sz = BUFFER_SIZE;
const unsigned int c_size = DATA_SIZE;

/*
    Vector Addition Kernel Implementation 
    Arguments:
        in1   (input)     --> Input Vector1
        in2   (input)     --> Input Vector2
        out   (output)    --> Output Vector
        size  (input)     --> Size of Vector in Integer
   */
extern "C" {
void vadd(
        const unsigned int *in1, // Read-Only Vector 1
        const unsigned int *in2, // Read-Only Vector 2
        unsigned int *out,       // Output Result
        int size                   // Size in integer
        )
{
#pragma HLS INTERFACE m_axi port=in1  offset=slave bundle=gmem
#pragma HLS INTERFACE m_axi port=in2  offset=slave bundle=gmem
#pragma HLS INTERFACE m_axi port=out offset=slave bundle=gmem
#pragma HLS INTERFACE s_axilite port=in1  bundle=control
#pragma HLS INTERFACE s_axilite port=in2  bundle=control
#pragma HLS INTERFACE s_axilite port=out bundle=control
#pragma HLS INTERFACE s_axilite port=size bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

    unsigned int v1_buffer[BUFFER_SIZE];    // Local memory to store vector1
    unsigned int v2_buffer[BUFFER_SIZE];    // Local memory to store vector2
    unsigned int vout_buffer[BUFFER_SIZE];  // Local Memory to store result


    //Per iteration of this loop perform BUFFER_SIZE vector addition
    for(int i = 0; i < size;  i += BUFFER_SIZE)
    {
    #pragma HLS LOOP_TRIPCOUNT min=c_size/c_chunk_sz max=c_size/c_chunk_sz
        int chunk_size = BUFFER_SIZE;
        //boundary checks
        if ((i + BUFFER_SIZE) > size) 
            chunk_size = size - i;

        // burst read of v1 and v2 vector from global memory
        read1: for (int j = 0 ; j < chunk_size ; j++){
        #pragma HLS LOOP_TRIPCOUNT min=c_chunk_sz max=c_chunk_sz
            v1_buffer[j] = in1[i + j];
        }
        read2: for (int j = 0 ; j < chunk_size ; j++){
        #pragma HLS LOOP_TRIPCOUNT min=c_chunk_sz max=c_chunk_sz
            v2_buffer[j] = in2[i + j];
        }

        //FPGA implementation, local array is mostly implemented as BRAM Memory block. 
        // BRAM Memory Block contains two memory ports which allow two parallel access 
        // to memory. To utilized both ports of BRAM block, vector addition loop is 
        // unroll with factor of 2. It is equivalent to following code:
        //  for (int j = 0 ; j < chunk_size ; j+= 2){
        //    vout_buffer[j]   = v1_buffer[j] + v2_buffer[j]; 
        //    vout_buffer[j+1] = v1_buffer[j+1] + v2_buffer[j+1]; 
        //  }
        //Which means two iterations of loop will be executed together and as a result 
        // it will double the performance.
        vadd: for (int j = 0 ; j < chunk_size; j ++){
        #pragma HLS PIPELINE II=1
        #pragma HLS UNROLL FACTOR=2
        #pragma HLS LOOP_TRIPCOUNT min=c_chunk_sz max=c_chunk_sz
            //perform vector addition
            vout_buffer[j] = v1_buffer[j] + v2_buffer[j]; 
        }

        //burst write the result
        write: for (int j = 0 ; j < chunk_size ; j++){
        #pragma HLS LOOP_TRIPCOUNT min=c_chunk_sz max=c_chunk_sz
            out[i + j] = vout_buffer[j];
        }
    }
}
}
