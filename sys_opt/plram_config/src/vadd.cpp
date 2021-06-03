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

// Includes
#include <hls_vector.h>
#include <hls_stream.h>
#include "assert.h"

#define DATA_SIZE 256 * 1024

// TRIPCOUNT identifier
const int c_size = DATA_SIZE;

static void load_input(hls::vector<unsigned int, 16>* in,
                       hls::stream<hls::vector<unsigned int, 16> >& inStream,
                       int vSize) {
mem_rd:
    for (int i = 0; i < vSize; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
        inStream << in[i];
    }
}

static void compute_add(hls::stream<hls::vector<unsigned int, 16> >& in1_stream,
                        hls::stream<hls::vector<unsigned int, 16> >& in2_stream,
                        hls::stream<hls::vector<unsigned int, 16> >& out_stream,
                        int vSize) {
execute:
    for (int i = 0; i < vSize; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
        out_stream << (in1_stream.read() + in2_stream.read());
    }
}

static void store_result(hls::vector<unsigned int, 16>* out,
                         hls::stream<hls::vector<unsigned int, 16> >& out_stream,
                         int vSize) {
mem_wr:
    for (int i = 0; i < vSize; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
        out[i] = out_stream.read();
    }
}

extern "C" {

/*
    Vector Addition Kernel

    Arguments:
        in1  (input)  --> Input vector 1
        in2  (input)  --> Input vector 2
        out  (output) --> Output vector
        size (input)  --> Number of elements in vector
*/

void vadd(hls::vector<unsigned int, 16>* in1,
          hls::vector<unsigned int, 16>* in2,
          hls::vector<unsigned int, 16>* out,
          int size) {
#pragma HLS INTERFACE m_axi port = in1 bundle = gmem0
#pragma HLS INTERFACE m_axi port = in2 bundle = gmem1
#pragma HLS INTERFACE m_axi port = out bundle = gmem0

    static hls::stream<hls::vector<unsigned int, 16> > in1_stream("input_stream_1");
    static hls::stream<hls::vector<unsigned int, 16> > in2_stream("input_stream_2");
    static hls::stream<hls::vector<unsigned int, 16> > out_stream("output_stream");

    // Since 16 values are processed
    // in parallel per loop iteration, the for loop only needs to iterate 'size / 16' times.
    assert(size % 16 == 0);
    int vSize = size / 16;
#pragma HLS dataflow

    load_input(in1, in1_stream, vSize);
    load_input(in2, in2_stream, vSize);
    compute_add(in1_stream, in2_stream, out_stream, vSize);
    store_result(out, out_stream, vSize);
}
}
