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
OpenCL Kernel Example using AXI4-master interface to access window of data from 2D array
*******************************************************************************/
                      
//Includes 
#include "host.h"

// Tripcount identifiers
__constant int c_row = TILE_PER_ROW;
__constant int c_col = TILE_PER_COLUMN;
__constant int c_width = TILE_WIDTH;
__constant int c_height = TILE_HEIGHT;

//Declaring PIPE memory with Depth 16
pipe int infifo __attribute__((xcl_reqd_pipe_depth(16)));
pipe int outfifo __attribute__((xcl_reqd_pipe_depth(16)));

// Read data Kernel : Read tile/window of Data from Global Memory
kernel __attribute__ ((reqd_work_group_size(1, 1, 1)))
void read_data(__global int *inx) {
    int tile[TILE_HEIGHT][TILE_WIDTH];
    __attribute__((xcl_loop_tripcount(c_col, c_col)))
    rd_loop_i: for(int i = 0; i < TILE_PER_COLUMN; ++i) {
        __attribute__((xcl_loop_tripcount(c_row, c_row)))
        rd_loop_j: for (int j = 0; j < TILE_PER_ROW; ++j) {
            __attribute__((xcl_loop_tripcount(c_height, c_height)))
            rd_buf_loop_m: for (int m = 0; m < TILE_HEIGHT; ++m) {
                __attribute__((xcl_pipeline_loop(1)))
                __attribute__((xcl_loop_tripcount(c_width, c_width)))
                rd_buf_loop_n: for (int n = 0; n < TILE_WIDTH; ++n) {
                    // should burst TILE_WIDTH in WORD beat
                    tile[m][n] = inx[TILE_HEIGHT*TILE_PER_ROW*TILE_WIDTH*i+TILE_PER_ROW*TILE_WIDTH*m+TILE_WIDTH*j+n];
                }
            }
            __attribute__((xcl_loop_tripcount(c_height, c_height)))
            rd_loop_m: for (int m = 0; m < TILE_HEIGHT; ++m) {
                __attribute__((xcl_pipeline_loop(1)))
                __attribute__((xcl_loop_tripcount(c_width, c_width)))
                rd_loop_n: for (int n = 0; n < TILE_WIDTH; ++n) {
                    write_pipe_block(infifo, &tile[m][n]);
                }
            }
        }
    }
}

// Write data Kernel : Write tile/window of Results to Global Memory
kernel __attribute__ ((reqd_work_group_size(1, 1, 1)))
void write_data(__global int *outx) {
    int tile[TILE_HEIGHT][TILE_WIDTH];
    __attribute__((xcl_loop_tripcount(c_col, c_col)))
    wr_loop_i: for(int i = 0; i < TILE_PER_COLUMN; ++i) {
        __attribute__((xcl_loop_tripcount(c_row, c_row)))
        wr_loop_j: for (int j = 0; j < TILE_PER_ROW; ++j) {
            __attribute__((xcl_loop_tripcount(c_height, c_height)))
            wr_buf_loop_m: for (int m = 0; m < TILE_HEIGHT; ++m) {
                __attribute__((xcl_pipeline_loop(1)))
                __attribute__((xcl_loop_tripcount(c_width, c_width)))
                wr_buf_loop_n: for (int n = 0; n < TILE_WIDTH; ++n) {
                    // should burst TILE_WIDTH in WORD beat
                    read_pipe_block(outfifo, &tile[m][n]);
                }
            }
            __attribute__((xcl_loop_tripcount(c_height, c_height)))
            wr_loop_m: for (int m = 0; m < TILE_HEIGHT; ++m) {
                __attribute__((xcl_pipeline_loop(1)))
                __attribute__((xcl_loop_tripcount(c_width, c_width)))
                wr_loop_n: for (int n = 0; n < TILE_WIDTH; ++n) {
                    outx[TILE_HEIGHT*TILE_PER_ROW*TILE_WIDTH*i+TILE_PER_ROW*TILE_WIDTH*m+TILE_WIDTH*j+n] = tile[m][n];
                }
            }
        }
    }
}

// Compute kernel, currently as simple as possible because this example is focused on efficient memory access pattern.
kernel __attribute__ ((reqd_work_group_size(1, 1, 1)))
void compute(int alpha) {
    __attribute__((xcl_loop_tripcount(c_col*c_height, c_col*c_height)))
    compute_loop_i: for(int i = 0; i < TILE_PER_COLUMN*TILE_HEIGHT; ++i) {
        __attribute__((xcl_loop_tripcount(c_row, c_row)))
        compute_loop_j: for (int jj = 0; jj < TILE_PER_ROW; ++jj) {
            __attribute__((xcl_pipeline_loop(1)))
            __attribute__((xcl_loop_tripcount(c_width, c_width)))
            compute_loop_m: for (int m = 0; m < TILE_WIDTH; ++m) {
                int inTmp;
                read_pipe_block(infifo, &inTmp);
                int outTmp = inTmp * alpha;
                write_pipe_block(outfifo, &outTmp);
            }
        }
    }
}
