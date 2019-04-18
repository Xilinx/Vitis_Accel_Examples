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
    HLS Dataflow Example using HLS Stream datatype 
    This is example of vector addition to demonstrate HLS Dataflow Pragma 
    functionality to perform task level parallelism using HLS Stream datatype. HLS 
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

//Includes 
#include <hls_stream.h>
#include <ap_int.h>

#define DATA_SIZE 4096

//TRIPCOUNT identifier
const int c_size = DATA_SIZE;

// Read Data from Global Memory and write into Stream inStream
static void read_input(unsigned int *in, hls::stream<unsigned int> &inStream,
        int size)
{
    mem_rd: for (int i = 0 ; i < size ; i++){
    #pragma HLS PIPELINE II=1
    #pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
        //Blocking write command to inStream 
        inStream << in[i];
    }
}

// Read Input data from inStream and write the result into outStream
static void compute_add(hls::stream<unsigned int> &inStream , 
        hls::stream<unsigned int> &outStream, int inc, int size)
{
    execute: for (int i = 0 ; i < size ; i++){
    #pragma HLS PIPELINE II=1
    #pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
        //Blocking read command from inStream and Blocking write command 
        //to outStream 
        outStream << (inStream.read() + inc);
    }
}

// Read result from outStream and write the result to Global Memory
static void write_result(unsigned int *out, hls::stream<unsigned int> 
        &outStream , int size)
{
    mem_wr: for (int i = 0 ; i < size ; i++){
    #pragma HLS PIPELINE II=1
    #pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
        //Blocking read command to inStream 
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
void adder(unsigned int *in, unsigned int *out, int inc, int size)
{
#pragma HLS INTERFACE m_axi port=in  offset=slave bundle=gmem
#pragma HLS INTERFACE m_axi port=out offset=slave bundle=gmem
#pragma HLS INTERFACE s_axilite port=in  bundle=control
#pragma HLS INTERFACE s_axilite port=out bundle=control
#pragma HLS INTERFACE s_axilite port=inc bundle=control
#pragma HLS INTERFACE s_axilite port=size bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

//Adding names for the streams. It allows the name to be used in reporting. Vivado HLS
//automatically checks to ensure all elements from an input stream are read during sw emulation.
    static hls::stream<unsigned int> inStream("input_stream");
    static hls::stream<unsigned int> outStream("output_stream");
#pragma HLS STREAM variable=inStream  depth=32
#pragma HLS STREAM variable=outStream depth=32
//  HLS STREAM variable=<name> depth=<size> pragma is used to define the Stream 
//  depth. For this example, Depth 32 is defined. Which means that Stream can hold 
//  maximum 32 outstanding elements at a given time. If Stream is full, any further
//  blocking write command from producer will go into wait state until consumer 
//  reads some elements from stream. Similarly if Stream is empty (no element in Stream)
//  any blocking read command from consumer will go into wait state until producer
//  writes elements to Stream. This blocking read and write allow consumer and 
//  producer to synchronize each other.

#pragma HLS dataflow
    //dataflow pragma instruct compiler to run following three APIs in parallel
    read_input(in,inStream,size);
    compute_add(inStream,outStream,inc,size);
    write_result(out,outStream,size);
}
}
