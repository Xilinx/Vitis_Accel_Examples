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
    This Example to demonstrate HLS Dependence Pragma. Using HLS DEPENDECE
pragma
    user can provide additional dependency details to compiler which allow
    compiler to implement unrolling/pipelining with optimized way.
*******************************************************************************/
#include "vconv.h"
#include <hls_stream.h>
#include <iostream>

// Read Data from DDR Memory and write into Stream inStream
static void read_input(int* in_r, hls::stream<int>& inStream, int size) {
// Auto-pipeline is going to apply pipeline to these loops
mem_rd:
    for (int i = 0; i < size; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
        // Blocking write command to inStream
        inStream << in_r[i];
    }
}

// Read Input data from inStream and write the result into outStream
static void vconv_compute(hls::stream<int>& inStream, hls::stream<int>& outStream, int height, int width) {
    int linebuf[K - 1][MAX_COLS] = {};
#pragma HLS ARRAY_PARTITION variable = linebuf dim = 1 complete
VConvH:
    for (int col = 0; col < height; ++col) {
    VConvW:
        for (int row = 0; row < width; ++row) {
#pragma HLS DEPENDENCE variable = linebuf inter false
            // HLS Dependence pragma provides extra dependency information to
            // compiler.
            // For example here linebuf has false inter dependency. Which means
            // each iteration of loop is independent for linebuf access.
            // It allows compiler to ignore dependency of linebuf and generate
            // a pipeline with lower II count.

            // If user does not provide this pragma, by default compiler assumes
            // dependency exists on linebuf between iterations. As a result,
            // compiler schedules unoptimized design with larger II factor
            // which in turn leads to slow performance.
            int in_val = inStream.read();
            int out_val = 0;
        VConv:
            for (int i = 0; i < K; i++) {
                int vwin_val = i < (K - 1) ? linebuf[i][row] : in_val;
                out_val += vwin_val * vcoeff[i];

                if (i > 0) linebuf[i - 1][row] = vwin_val;
            }
            outStream << out_val;
        }
    }
}

// Read result from outStream and write the result to Global Memory
static void write_result(int* out_r, hls::stream<int>& outStream, int size) {
mem_wr:
    for (int i = 0; i < size; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
        // Blocking read command to outStream
        out_r[i] = outStream.read();
    }
}

extern "C" {
void vconv(int* in_r, int* out_r, int height, int width) {
    int size = height * width;

    // inStream for reading input data
    static hls::stream<int> inStream;
#pragma HLS stream variable = inStream depth = 32

    // outStream for writing data to Global Memory
    static hls::stream<int> outStream;
#pragma HLS stream variable = outStream depth = 32

#pragma HLS dataflow
    // dataflow pragma instruct compiler to run following three APIs in parallel
    read_input(in_r, inStream, size);
    vconv_compute(inStream, outStream, height, width);
    write_result(out_r, outStream, size);
}
}
