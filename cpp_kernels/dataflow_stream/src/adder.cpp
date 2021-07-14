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
    HLS Dataflow Example using HLS Stream datatype
    This is example of vector addition to demonstrate HLS Dataflow Pragma
    functionality to perform task level parallelism using HLS Stream datatype.
HLS
    dataflow pragma instruct compiler to run sub-task parallel. Sub-task can
    transfer the data using hls stream. In this Example, a vector addition
    implementation is divided into three sub-task APIs as below:

    1) read_input():
        This API reads the input vector from Global Memory and writes it into
        HLS Stream 'inStream' using blocking write command.

    2) compute_add():
        This API reads the input vector from 'inStream' using blocking read
        command and increment the value by user specified increment. It writes
        the results into 'outStream' using blocking write command.

    3) write_result():
        This API reads the result vector from 'outStream' using blocking read
        command and write the result into Global Memory Location.

    Data Flow Stream based Adder will be implemented as below:
                    _____________
                    |             |<----- Input Vector from Global Memory
                    |  read_input |       __
                    |_____________|----->|  |
                     _____________       |  | inStream
                    |             |<-----|__|
                    | compute_add |       __
                    |_____________|----->|  |
                     _____________       |  | outStream
                    |              |<----|__|
                    | write_result |
                    |______________|-----> Output result to Global Memory


*******************************************************************************/

// Includes
#include <ap_int.h>
#include <hls_stream.h>

#define DATA_SIZE 4096

// TRIPCOUNT identifier
const int c_size = DATA_SIZE;

// Read Data from Global Memory and write into Stream inStream
static void read_input(unsigned int* in, hls::stream<unsigned int>& inStream, int size) {
// Auto-pipeline is going to apply pipeline to this loop
mem_rd:
    for (int i = 0; i < size; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
        // Blocking write command to inStream
        inStream << in[i];
    }
}

// Read Input data from inStream and write the result into outStream
static void compute_add(hls::stream<unsigned int>& inStream, hls::stream<unsigned int>& outStream, int inc, int size) {
// Auto-pipeline is going to apply pipeline to this loop
execute:
    for (int i = 0; i < size; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
        // Blocking read command from inStream and Blocking write command
        // to outStream
        outStream << (inStream.read() + inc);
    }
}

// Read result from outStream and write the result to Global Memory
static void write_result(unsigned int* out, hls::stream<unsigned int>& outStream, int size) {
// Auto-pipeline is going to apply pipeline to this loop
mem_wr:
    for (int i = 0; i < size; i++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
        // Blocking read command to inStream
        out[i] = outStream.read();
    }
}

extern "C" {
/*
    Vector Addition Kernel Implementation using dataflow
    Arguments:
        in   (input)  --> Input Vector
        out  (output) --> Output Vector
        inc  (input)  --> Increment
        size (input)  --> Size of Vector in Integer
   */
void adder(unsigned int* in, unsigned int* out, int inc, int size) {
    // Adding names for the streams. It allows the name to be used in reporting.
    // Vivado HLS
    // automatically checks to ensure all elements from an input stream are read
    // during sw emulation.
    static hls::stream<unsigned int> inStream("input_stream");
    static hls::stream<unsigned int> outStream("output_stream");
#pragma HLS STREAM variable = inStream depth = 32
#pragma HLS STREAM variable = outStream depth = 32
//  HLS STREAM variable=<name> depth=<size> pragma is used to define the Stream
//  depth. For this example, Depth 32 is defined. Which means that Stream can
//  hold
//  maximum 32 outstanding elements at a given time. If Stream is full, any
//  further
//  blocking write command from producer will go into wait state until consumer
//  reads some elements from stream. Similarly if Stream is empty (no element in
//  Stream)
//  any blocking read command from consumer will go into wait state until
//  producer
//  writes elements to Stream. This blocking read and write allow consumer and
//  producer to synchronize each other.

#pragma HLS dataflow
    // dataflow pragma instruct compiler to run following three APIs in parallel
    read_input(in, inStream, size);
    compute_add(inStream, outStream, inc, size);
    write_result(out, outStream, size);
}
}
