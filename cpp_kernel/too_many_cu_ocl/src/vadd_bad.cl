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
    good performance.

*******************************************************************************/

/*

Kernel Description (Bad Example) :
   
    This example performs vector addition and uses multiple compute units to
    achieve parallelism.
        
    Note :  This is the Naive version (Bad Example) version of the vadd kernel.
            The single CU (Good Example) is in the file vadd.cl
    
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

// Tripcount identifiers
__constant int c_size = BUFFER_SIZE;

__kernel __attribute__ ((reqd_work_group_size(1, 1, 1)))
void vadd_BAD(
        const __global int *in1, // Read-Only Vector 1
        const __global int *in2, // Read-Only Vector 2
        __global int *out,       // Output Result
        int size                 // Size of Vector Data
        )
{
    int global_id   = get_global_id(0);         // Picks Work_Group ID
    int global_size = get_global_size(0);       // Picks total Work_Item count

    // Local memory to store input and output vectors
    // Local memory is implemented as BRAM memory blocks
    // SDx assigns an independent local buffers for each compute unit
    // For example : if 8 compute units are used, each compute unit gets
    // three local buffers, a total 3*8 = 24 local buffers are allocated.
    int in1_lcl[BUFFER_SIZE];
    int in2_lcl[BUFFER_SIZE];
    int out_lcl[BUFFER_SIZE];
    
    int chunk_size = BUFFER_SIZE;

    // Computes vector addition operation iteratively over entire data set
    for(int offset = 0; offset < size; offset += chunk_size*global_size){
        
        // Enables burst reads on input vectors from global memory
        // Each Work_Item asynchronously moves its work load from global memory
        // to local memory (in1_lcl, in2_lcl) associated per each Work_Group

        //boundary checks
        if ((offset + chunk_size) > size) 
            chunk_size = size - offset;
        
        // Burst read for in1_lcl
        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_size, c_size)))
        readIn1: for(int j = 0 ; j < chunk_size; j++){
            in1_lcl[j] = in1[global_id*BUFFER_SIZE + offset + j]; 
        }
        
        // Burst read for in2_lcl
        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_size, c_size)))
        readIn2: for(int j = 0 ; j < chunk_size; j++){
            in2_lcl[j] = in2[global_id*BUFFER_SIZE + offset + j]; 
        }
        
        // Performs vector addition.
        // Vector addition is operated on elements across multiple 
        // compute units in parallel.

        // This computation results in SIMD (Single Instruction Multiple
        // Data) type of execution

        // Total 8 compute units can be allocated since 8 Work_Groups are
        // present in the kernel specification.

        // Pipeline Operations
        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_size, c_size)))
        vadd: for(int i = 0; i < BUFFER_SIZE; i++){
            out_lcl[i] = in1_lcl[i] + in2_lcl[i];
        }

        // Burst write from kernel to global memory on "out" vector
        // Each Work_Item writes back results from local buffers to global memory
        // based on Work_Group which it belongs to.
        
        // Burst write from out_lcl
        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_size, c_size)))
        writeOut: for(int j = 0 ; j < chunk_size; j++){
            out[global_id*BUFFER_SIZE + offset + j] = out_lcl[j];
        }
    }
    return;
}
