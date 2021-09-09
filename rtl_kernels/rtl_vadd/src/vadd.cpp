/**
* Copyright (C) 2019-2021 Xilinx, Inc
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

    This example uses the load/compute/store coding style which is generally
    the most efficient for implementing kernels using HLS. The load and store
    functions are responsible for moving data in and out of the kernel as
    efficiently as possible. The core functionality is decomposed across one
    of more compute functions. Whenever possible, the compute function should
    pass data through HLS streams and should contain a single set of nested loops.

    HLS stream objects are used to pass data between producer and consumer
    functions. Stream read and write operations have a blocking behavior which
    allows consumers and producers to synchronize with each other automatically.

    The dataflow pragma instructs the compiler to enable task-level pipelining.
    This is required for to load/compute/store functions to execute in a parallel
    and pipelined manner. Here the kernel loads, computes and stores integer values per
    clock cycle and is implemented as below:
                                       _____________
                                      |             |<----- Input Vector 1 from Global Memory
                                      |  load_input |       __
                                      |_____________|----->|  |
                                       _____________       |  | a_stream
Input Vector 2 from Global Memory --->|             |      |__|
                               __     |  load_input |        |
                              |  |<---|_____________|        |
                     b_stream |  |     _____________         |
                              |__|--->|             |<--------
                                      | compute_add |      __
                                      |_____________|---->|  |
                                       ______________     |  | out_stream
                                      |              |<---|__|
                                      | store_result |
                                      |______________|-----> Output result to Global Memory

*******************************************************************************/

// Includes
#include <hls_vector.h>
#include <hls_stream.h>
#include "ap_int.h"

#define DATA_SIZE 4096

// TRIPCOUNT identifier
const int c_size = DATA_SIZE;

static void load_input(uint32_t* in, hls::stream<uint32_t>& inStream, ap_uint<32> vSize) {
mem_rd:
    for (int i = 0; i < vSize; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
        inStream << in[i];
    }
}

static void compute_add(hls::stream<uint32_t>& a_stream,
                        hls::stream<uint32_t>& b_stream,
                        hls::stream<uint32_t>& c_stream,
                        ap_uint<32> vSize) {
execute:
    for (int i = 0; i < vSize; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
        c_stream << (a_stream.read() + b_stream.read());
    }
}

static void store_result(uint32_t* out, hls::stream<uint32_t>& out_stream, ap_uint<32> vSize) {
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
        a  (input)  --> Input vector 1
        b  (input)  --> Input vector 2
        c  (output) --> Output vector
        length_r (input)  --> Number of elements in vector
*/

void krnl_vadd_rtl(uint32_t* a, uint32_t* b, uint32_t* c, ap_uint<32> length_r) {
#pragma HLS INTERFACE m_axi port = a bundle = gmem
#pragma HLS INTERFACE m_axi port = b bundle = gmem
#pragma HLS INTERFACE m_axi port = c bundle = gmem
#pragma HLS INTERFACE ap_ctrl_hs port = return

    static hls::stream<uint32_t> a_stream("input_stream_1");
    static hls::stream<uint32_t> b_stream("input_stream_2");
    static hls::stream<uint32_t> c_stream("output_stream");

#pragma HLS dataflow

    load_input(a, a_stream, length_r);
    load_input(b, b_stream, length_r);
    compute_add(a_stream, b_stream, c_stream, length_r);
    store_result(c, c_stream, length_r);
}
}
