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
C Kernel Example using AXI4-master interface to access window of data from 2D array
*******************************************************************************/

//Includes 
#include "host.h"
#include <hls_stream.h>

// use HLS stream library for easy use of AXI-stream interface
typedef hls::stream<DTYPE> my_data_fifo;

// Read data function : Read tile/window of Data from Global Memory
void read_data(DTYPE *inx, my_data_fifo &inFifo) {
    DTYPE tile[TILE_HEIGHT][TILE_WIDTH];
    rd_loop_i: for(int i = 0; i < TILE_PER_COLUMN; ++i) {
        rd_loop_j: for (int j = 0; j < TILE_PER_ROW; ++j) {
        #pragma HLS DATAFLOW
            rd_buf_loop_m: for (int m = 0; m < TILE_HEIGHT; ++m) {
                rd_buf_loop_n: for (int n = 0; n < TILE_WIDTH; ++n) {
                #pragma HLS PIPELINE II=1
                    // should burst TILE_WIDTH in WORD beat
                    tile[m][n] = inx[TILE_HEIGHT*TILE_PER_ROW*TILE_WIDTH*i+TILE_PER_ROW*TILE_WIDTH*m+TILE_WIDTH*j+n];
                }
            }
            rd_loop_m: for (int m = 0; m < TILE_HEIGHT; ++m) {
                rd_loop_n: for (int n = 0; n < TILE_WIDTH; ++n) {
                #pragma HLS PIPELINE II=1
                    inFifo << tile[m][n];
                }
            }
        }
    }
}

// Write data function : Write tile/window of Results to Global Memory
void write_data(DTYPE *outx, my_data_fifo &outFifo) {
    DTYPE tile[TILE_HEIGHT][TILE_WIDTH];
    wr_loop_i: for(int i = 0; i < TILE_PER_COLUMN; ++i) {
        wr_loop_j: for (int j = 0; j < TILE_PER_ROW; ++j) {
        #pragma HLS DATAFLOW
            wr_buf_loop_m: for (int m = 0; m < TILE_HEIGHT; ++m) {
                wr_buf_loop_n: for (int n = 0; n < TILE_WIDTH; ++n) {
                #pragma HLS PIPELINE II=1
                    // should burst TILE_WIDTH in WORD beat
                    outFifo >> tile[m][n];
                }
            }
            wr_loop_m: for (int m = 0; m < TILE_HEIGHT; ++m) {
                wr_loop_n: for (int n = 0; n < TILE_WIDTH; ++n) {
                #pragma HLS PIPELINE II=1
                    outx[TILE_HEIGHT*TILE_PER_ROW*TILE_WIDTH*i+TILE_PER_ROW*TILE_WIDTH*m+TILE_WIDTH*j+n] = tile[m][n];
                }
            }
        }
    }
}

// Compute function, currently as simple as possible because this example is focused on efficient memory access pattern.
void compute(my_data_fifo &inFifo, my_data_fifo &outFifo, int alpha) {
    compute_loop_i: for(int i = 0; i < TILE_PER_COLUMN*TILE_HEIGHT; ++i) {
        compute_loop_j: for (int j = 0; j < TILE_PER_ROW; ++j) {
            compute_loop_m: for (int m = 0; m < TILE_WIDTH; ++m) {
            #pragma HLS PIPELINE II=1
                DTYPE inTmp;
                inFifo >> inTmp;
                DTYPE outTmp = inTmp * alpha;
                outFifo << outTmp;
            }
        }
    }
}

extern "C" {
    void window_array_2d(DTYPE *inx, DTYPE *outx, int alpha) {
// AXI master interface
#pragma HLS INTERFACE m_axi port=inx offset=slave bundle=gmem 
#pragma HLS INTERFACE m_axi port=outx offset=slave bundle=gmem 
// AXI slave interface
#pragma HLS INTERFACE s_axilite port=inx bundle=control
#pragma HLS INTERFACE s_axilite port=outx bundle=control
#pragma HLS INTERFACE s_axilite port=alpha bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

        my_data_fifo inFifo;
#pragma HLS stream variable=inFifo depth=32

        my_data_fifo outFifo;
#pragma HLS stream variable=outFifo depth=32

// Enables task level pipelining, allowing functions and loops to execute concurrently. Used to minimize interval. More details please refer to UG902.
#pragma HLS DATAFLOW
        // Read data from 2D array using tile/window pattern
        read_data(inx, inFifo);
        // Do computation with the acquired data
        compute(inFifo, outFifo, alpha);
        // Write data to 2D array using tile/window pattern
        write_data(outx, outFifo);
        return;
    }
}
