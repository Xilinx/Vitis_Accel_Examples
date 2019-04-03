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
OpenCL Kernel Example using AXI4-master interface to access row of data from 2D array
*******************************************************************************/
                      
//Includes 
#include "host.h"

// Tripcount identifiers
__constant int c_rows = NUM_ROWS;
__constant int c_size = WORD_PER_ROW;

// Read data function : Read Data from Global Memory
void read_data(__global int* inx, int* buffer_in) 
{
    __attribute__((xcl_loop_tripcount(c_rows, c_rows)))
    read_loop_i: for(int i = 0; i < NUM_ROWS; ++i) {
        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_size, c_size)))
        read_loop_jj: for (int j = 0; j < WORD_PER_ROW; ++j) {
            buffer_in[WORD_PER_ROW*i+j] = inx[WORD_PER_ROW*i+j];
        }
    }
}

// Write data function : Write Results to Global Memory
void write_data(__global int* outx, int* buffer_out) 
{
    __attribute__((xcl_loop_tripcount(c_rows, c_rows)))
    write_loop_i: for(int i = 0; i < NUM_ROWS; ++i) {
        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_size, c_size)))
        write_loop_jj: for (int j = 0; j < WORD_PER_ROW; ++j) {
            outx[WORD_PER_ROW*i+j] = buffer_out[WORD_PER_ROW*i+j];
        }
    }
}

// Compute function, currently as simple as possible because this example is focused on efficient memory access pattern.
void compute(int alpha, int* buffer_in, int* buffer_out) 
{
    __attribute__((xcl_loop_tripcount(c_rows, c_rows)))
    compute_loop_i: for(int i = 0; i < NUM_ROWS; ++i) {
        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_size, c_size)))
        compute_loop_jj: for (int j = 0; j < WORD_PER_ROW; ++j) {
            int inTmp = buffer_in[WORD_PER_ROW*i+j];
            int outTmp = inTmp * alpha;
            buffer_out[WORD_PER_ROW*i+j] = outTmp;
        }
    }
}

__kernel 
__attribute__ ((reqd_work_group_size(1, 1, 1)))
__attribute__ ((xcl_dataflow))
// Top Kernel to call sub-functions
void row_array_2d(__global int* inx, __global int* outx, int alpha) 
{
    int buffer_in[BLOCK_SIZE];
    int buffer_out[BLOCK_SIZE];
    read_data(inx, buffer_in);
    compute(alpha, buffer_in, buffer_out);
    write_data(outx, buffer_out);
    return;
}
