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

SDx Key Concept :
    
    This is a vector addition based example which showcases the effectiveness of
    using heavier single compute unit instead of multiple compute units to achieve 
    similar performance.    

*******************************************************************************/

/*

Kernel Description (Good Example) :
   
    This example uses array partition and unrolling (automatic) to showcase higher
    parallelism with a single compute unit and achieves similar performance compared
    to naive implementation which uses multiple compute units to achieve better 
    performance. 
    
    Note : Naive version (Bad Example) version is in the file vadd_bad.cpp
    
    Arguments :
    
        int *in1   (input ) --> Input Vector - 1
        int *in2   (input ) --> Input Vector - 2
        int *out   (output) --> Output Vector
        int  size  (input ) --> Size of Vector

    Kernel Configuration :
        
        1. Vector Size  = 81920
        2. Work Groups  = 8
        3. Work Items   = 8     ( One per each Work_Group )
        4. Local Buffer = 5120  ( Work_Item Load )


*/

#include<stdio.h>

// Work load of each Work_Item
#define BUFFER_SIZE 5120
#define DATA_SIZE 81920
//Number of Compute Units (CU)
#define NUM_CU 8

//TRIPCOUNT indentifier
const int c_size = DATA_SIZE/(BUFFER_SIZE*NUM_CU);

typedef unsigned int uint;

extern "C"
{
    void vadd_GOOD(
            const int *in1, // Read-Only Vector 1
            const int *in2, // Read-Only Vector 2
            int *out,       // Output Result
            int size        // Size of Vector Data
    )
    {
    #pragma HLS INTERFACE m_axi port=in1 offset=slave bundle=gmem
    #pragma HLS INTERFACE m_axi port=in2 offset=slave bundle=gmem
    #pragma HLS INTERFACE m_axi port=out offset=slave bundle=gmem

    #pragma HLS INTERFACE s_axilite port=in1 bundle=control
    #pragma HLS INTERFACE s_axilite port=in2 bundle=control
    #pragma HLS INTERFACE s_axilite port=out bundle=control
    #pragma HLS INTERFACE s_axilite port=size bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control
    
        // Local memory to store input and output vectors
        // Local memory is implemented as BRAM memory blocks
        int in1_lcl[NUM_CU][BUFFER_SIZE];
        #pragma HLS ARRAY_PARTITION variable=in1_lcl dim=1 complete

        int in2_lcl[NUM_CU][BUFFER_SIZE];
        #pragma HLS ARRAY_PARTITION variable=in2_lcl dim=1 complete
        
        int out_lcl[NUM_CU][BUFFER_SIZE];
        #pragma HLS ARRAY_PARTITION variable=out_lcl dim=1 complete
        
        // Computes vector addition operation iteratively over entire data set of the work item
        for(int offset = 0; offset < size; offset += NUM_CU*BUFFER_SIZE){
        #pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
            // Enables burst reads on input vectors from global memory
            // Each Work_Item asynchronously moves its work load from global memory
            // to local memory (in1_lcl, in2_lcl) associated per each Work_Group
            
            // Burst read for in1_lcl
            readIn1: for(int itr = 0 , i = 0 , j =0; itr < NUM_CU*BUFFER_SIZE; itr++, j++){
            #pragma HLS PIPELINE II=1
                if(j == BUFFER_SIZE) { j = 0 ; i++; }
                in1_lcl[i][j] = in1[offset + itr];
            }
            
            // Burst read for in2_lcl
            readIn2: for(int itr = 0 , i = 0 , j =0; itr < NUM_CU*BUFFER_SIZE; itr++, j++){
            #pragma HLS PIPELINE II=1
                if(j == BUFFER_SIZE) { j = 0 ; i++; }
                in2_lcl[i][j] = in2[offset + itr];
            }

            // Performs vector addition.

            // This computation results in SIMD (Single Instruction Multiple
            // Data) style of execution over NUM_CU (8) elements

            // Unroll increases the work load and helps in achieving parallelism 

            // SDx generates highly optimized design for vector addition in
            // this Single compute unit (Good Example) example due to high work load
            // per iteration / compute unit and parallelism exposed through array 
            // partition and unroll.
            
            // Unroll & Pipeline - The inner loop vadd2 is unrolled automatically.
            // Note that the loop order is changed. This is done since the design
            // is to do 8 (NUM_CU) operations and compute 8 results in parallel.
            
            vadd1: for(int i = 0; i < BUFFER_SIZE; i++){
            #pragma HLS PIPELINE II=1
                vadd2: for(int j = 0; j < NUM_CU; j++){
                    out_lcl[j][i] = in1_lcl[j][i] + in2_lcl[j][i];
                }
            }

            // Burst write from kernel to global memory on "out" vector
            // Each Work_Item writes back results from local buffers to global memory
            // based on Work_Group which it belongs to.
            
            // Burst write from out_lcl
            writeOut: for(int itr = 0 , i = 0 , j =0; itr < NUM_CU*BUFFER_SIZE; itr++, j++){
            #pragma HLS PIPELINE II=1
                if(j == BUFFER_SIZE) { j = 0 ; i++; }
                out[offset + itr] = out_lcl[i][j];
            }
        }
        return;
    }
}
