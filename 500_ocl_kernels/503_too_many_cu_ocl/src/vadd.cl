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
    
    Note : Naive version (Bad Example) version is in the file vadd_bad.cl
    
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

// Work load of each Work_Item
#define BUFFER_SIZE 5120

//Number of Compute Units (CU)
#define NUM_CU 8

// Tripcount identifiers
__constant int c_size = BUFFER_SIZE;
__constant int c_cu = NUM_CU;

__kernel __attribute__ ((reqd_work_group_size(1, 1, 1)))
void vadd_GOOD(
        const __global int *in1, // Read-Only Vector 1
        const __global int *in2, // Read-Only Vector 2
        __global int *out,       // Output Result
        int size                 // Size of Vector Data
        )
{
    int global_id    = get_global_id(0);         // Picks Work_Group ID
    int global_size  = get_global_size(0);       // Picks total Work_Item count

    // Local memory to store input and output vectors
    // Local memory is implemented as BRAM memory blocks
    int in1_lcl[NUM_CU][BUFFER_SIZE] __attribute__((xcl_array_partition(complete, 1)));
    int in2_lcl[NUM_CU][BUFFER_SIZE] __attribute__((xcl_array_partition(complete, 1)));
    int out_lcl[NUM_CU][BUFFER_SIZE] __attribute__((xcl_array_partition(complete, 1)));
 
    int chunk_size = NUM_CU*BUFFER_SIZE;
    // Computes vector addition operation iteratively over entire data set of the work item
    for(int offset = 0; offset < size; offset += chunk_size*global_size){
        
        // Enables burst reads on input vectors from global memory
        // Each Work_Item asynchronously moves its work load from global memory
        // to local memory (in1_lcl, in2_lcl) associated per each Work_Group
        
        //boundary checks
        if ((offset + chunk_size) > size) 
            chunk_size = size - offset;

        // Burst read for in1_lcl
        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_size*c_cu, c_size*c_cu)))
        readIn1: for(int itr = 0 , i = 0 , j =0; itr < chunk_size; itr++, j++){
            if(j == BUFFER_SIZE) { j = 0 ; i++; }
            in1_lcl[i][j] = in1[global_id*BUFFER_SIZE + offset + itr];
        }
        
        // Burst read for in2_lcl
        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_size*c_cu, c_size*c_cu)))
        readIn2: for(int itr = 0 , i = 0 , j =0; itr < chunk_size; itr++, j++){
            if(j == BUFFER_SIZE) { j = 0 ; i++; }
            in2_lcl[i][j] = in2[global_id*BUFFER_SIZE + offset + itr];
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
        
        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_size, c_size)))
        vadd1: for(int i = 0; i < BUFFER_SIZE; i++){
            __attribute__((xcl_loop_tripcount(c_cu, c_cu)))
            vadd2: for(int j = 0; j < NUM_CU; j++){
                out_lcl[j][i] = in1_lcl[j][i] + in2_lcl[j][i];
            }
        }

        // Burst write from kernel to global memory on "out" vector
        // Each Work_Item writes back results from local buffers to global memory
        // based on Work_Group which it belongs to.
        
        // Burst write from out_lcl
        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_size*c_cu, c_size*c_cu)))
        writeOut: for(int itr = 0 , i = 0 , j =0; itr < chunk_size; itr++, j++){
            if(j == BUFFER_SIZE) { j = 0 ; i++; }
            out[global_id*BUFFER_SIZE + offset + itr] = out_lcl[i][j];
        }
    }
    return;
}
