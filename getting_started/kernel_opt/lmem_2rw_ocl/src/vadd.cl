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
    OpenCL Kernel to showcase 2 parallel read/write from Local Memory 
    Description: This is vector addition to demonstrate how to utilized both
    ports of Local Memory. 
*******************************************************************************/

#define BUFFER_SIZE 1024
#define DATA_SIZE 4096

// Tripcount identifiers
__constant int c_size = DATA_SIZE/BUFFER_SIZE;
__constant int c_chunk_size = BUFFER_SIZE;

/*
    Vector Addition Kernel Implementation using uint datatype 
    Arguments:
        in1   (input)     --> Input Vector1
        in2   (input)     --> Input Vector2
        out   (output)    --> Output Vector
        size  (input)     --> Size of Vector in Integer
   */
kernel __attribute__ ((reqd_work_group_size(1, 1, 1)))
void vadd(
        const __global uint *in1, // Read-Only Vector 1
        const __global uint *in2, // Read-Only Vector 2
        __global uint *out,       // Output Result
        int size                   // Size in integer
        )
{
    local uint v1_buffer[BUFFER_SIZE];    // Local memory to store vector1
    local uint v2_buffer[BUFFER_SIZE];    // Local memory to store vector2
    local uint vout_buffer[BUFFER_SIZE];  // Local Memory to store result


    //Per iteration of this loop perform BUFFER_SIZE vector addition
    __attribute__((xcl_loop_tripcount(c_size, c_size)))
    for(int i = 0; i < size;  i += BUFFER_SIZE)
    {
        int chunk_size = BUFFER_SIZE;
        //boundary checks
        if ((i + BUFFER_SIZE) > size) 
            chunk_size = size - i;

        // burst read of v1 and v2 vector from global memory
        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_chunk_size, c_chunk_size)))
        read1: for (int j = 0 ; j < chunk_size; j++) v1_buffer[j] = in1[i+j];

        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_chunk_size, c_chunk_size)))
        read2: for (int j = 0 ; j < chunk_size; j++) v2_buffer[j] = in2[i+j];

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
        __attribute__((opencl_unroll_hint(2)))
        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_chunk_size, c_chunk_size)))
        vadd: for (int j = 0 ; j < chunk_size; j ++){
            //perform vector addition
            vout_buffer[j] = v1_buffer[j] + v2_buffer[j]; 
        }

        //burst write the result
        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_chunk_size, c_chunk_size)))
        write: for (int j = 0 ; j < chunk_size; j++) out[i+j] = vout_buffer[j];
    }
}
