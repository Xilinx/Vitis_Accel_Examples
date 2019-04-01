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
    OpenCL Dataflow Example using xcl_dataflow attribute 
    This is example of vector addition to demonstrate OpenCL Dataflow xcl_dataflow 
    functionality to perform task/subfunction level parallelism using xcl_dataflow
    attribute. OpenCL xcl_dataflow instruct compiler to run subfunctions inside kernel 
    concurrently. In this Example, the kernel calls run_subfunc API to perform vector 
    addition implementation and which inturn is divided into three sub-function APIs 
    as below:

    1) read_input(): 
        This API reads the input vector from Global Memory and writes it into 
        'buffer_in'.

    2) compute_add(): 
        This API reads the input vector from 'buffer_in' and increment the value 
        by user specified increment. It writes the result into 'buffer_out'.

    3) write_result(): 
        This API reads the result vector from 'buffer_out' and write the result 
        into Global Memory Location.

    Data Flow based Adder will be implemented as below:
                    _____________
                    |             |<----- Input Vector from Global Memory
                    |  read_input |       __
                    |_____________|----->|  |
                     _____________       |  | buffer_in
                    |             |<-----|__|
                    | compute_add |       __
                    |_____________|----->|  |
                     _____________       |  | buffer_out
                    |              |<----|__|
                    | write_result |       
                    |______________|-----> Output result to Global Memory


*******************************************************************************/
#define DATA_SIZE 4096
#define BUFFER_SIZE 4096

// Tripcount identifiers
__constant int c_size = DATA_SIZE;

//Includes 
// Read Data from Global Memory and write into buffer_in
static void read_input(__global int *in, int * buffer_in,
        int size)
{
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_size, c_size)))
    read: for (int i = 0 ; i < size ; i++){
        buffer_in[i] =  in[i];
    }
}

// Read Input data from buffer_in and write the result into buffer_out
static void compute_add(int * buffer_in , int * buffer_out
        , int inc, int size)
{
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_size, c_size)))
    compute: for (int i = 0 ; i < size ; i++){
        buffer_out[i] = buffer_in[i] + inc;
    }
}

// Read result from buffer_out and write the result to Global Memory
static void write_result(__global int *out, int* buffer_out,
        int size)
{
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_size, c_size)))
    write: for (int i = 0 ; i < size ; i++){
        out[i] = buffer_out[i];
    }
}

//Dataflow attribute instructs compiler to run following three 
//sub-functions concurrently
__attribute__ ((xcl_dataflow))
void run_subfunc(__global int *in, __global int *out, int inc, int size)
{
    int buffer_in[BUFFER_SIZE];
    int buffer_out[BUFFER_SIZE];

    read_input(in,buffer_in,size);
    compute_add(buffer_in,buffer_out,inc,size);
    write_result(out,buffer_out,size);
}

/*
    Vector Addition Kernel Implementation using dataflow in sub functions 
    Arguments:
        in   (input)  --> Input Vector
        out  (output) --> Output Vector
        inc  (input)  --> Increment
        size (input)  --> Size of Vector in Integer
*/
__kernel 
__attribute__ ((reqd_work_group_size(1, 1, 1)))
void adder(__global int *in, __global int *out, int inc, int size)
{
    run_subfunc(in, out, inc, size);
}
