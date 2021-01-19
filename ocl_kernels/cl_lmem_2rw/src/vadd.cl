/**
* Copyright (C) 2020 Xilinx, Inc
*
* Licensed under the Apache License, Version 2.0 (the "License"). You may
* not use this file except in compliance with the License. A copy of the
* License is located at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
* License for the specific language governing permissions and limitations
* under the License.
*/

/*******************************************************************************
Description:
    OpenCL Kernel to showcase 2 parallel read/write from Local Memory
    Description: This is vector addition to demonstrate how to utilized both
    ports of Local Memory.
*******************************************************************************/

#define BUFFER_SIZE 1024
#define DATA_SIZE 4096

// Tripcount identifiers
__constant int c_size = DATA_SIZE / BUFFER_SIZE;
__constant int c_chunk_size = BUFFER_SIZE;

/*
    Vector Addition Kernel Implementation using uint datatype
    Arguments:
        in1   (input)     --> Input Vector1
        in2   (input)     --> Input Vector2
        out_r   (output)    --> Output Vector
        size  (input)     --> Size of Vector in Integer
   */
kernel __attribute__((reqd_work_group_size(1, 1, 1))) void vadd(const __global uint* in1, // Read-Only Vector 1
                                                                const __global uint* in2, // Read-Only Vector 2
                                                                __global uint* out_r,     // Output Result
                                                                int size                  // Size in integer
                                                                ) {
    local uint v1_buffer[BUFFER_SIZE];   // Local memory to store vector1
    local uint v2_buffer[BUFFER_SIZE];   // Local memory to store vector2
    local uint vout_buffer[BUFFER_SIZE]; // Local Memory to store result

    // Per iteration of this loop perform BUFFER_SIZE vector addition
    __attribute__((xcl_loop_tripcount(c_size, c_size))) for (int i = 0; i < size; i += BUFFER_SIZE) {
        int chunk_size = BUFFER_SIZE;
        // boundary checks
        if ((i + BUFFER_SIZE) > size) chunk_size = size - i;

        // burst read of v1 and v2 vector from global memory
        __attribute__((xcl_pipeline_loop(1))) __attribute__((xcl_loop_tripcount(c_chunk_size, c_chunk_size))) read1
            : for (int j = 0; j < chunk_size; j++) v1_buffer[j] = in1[i + j];

        __attribute__((xcl_pipeline_loop(1))) __attribute__((xcl_loop_tripcount(c_chunk_size, c_chunk_size))) read2
            : for (int j = 0; j < chunk_size; j++) v2_buffer[j] = in2[i + j];

        // FPGA implementation, local array is mostly implemented as BRAM Memory
        // block.
        // BRAM Memory Block contains two memory ports which allow two parallel
        // access
        // to memory. To utilized both ports of BRAM block, vector addition loop is
        // unroll with factor of 2. It is equivalent to following code:
        //  for (int j = 0 ; j < chunk_size ; j+= 2){
        //    vout_buffer[j]   = v1_buffer[j] + v2_buffer[j];
        //    vout_buffer[j+1] = v1_buffer[j+1] + v2_buffer[j+1];
        //  }
        // Which means two iterations of loop will be executed together and as a
        // result
        // it will double the performance.
        __attribute__((opencl_unroll_hint(2))) __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_chunk_size, c_chunk_size))) vadd : for (int j = 0; j < chunk_size; j++) {
            // perform vector addition
            vout_buffer[j] = v1_buffer[j] + v2_buffer[j];
        }

        // burst write the result
        __attribute__((xcl_pipeline_loop(1))) __attribute__((xcl_loop_tripcount(c_chunk_size, c_chunk_size))) write
            : for (int j = 0; j < chunk_size; j++) out_r[i + j] = vout_buffer[j];
    }
}
