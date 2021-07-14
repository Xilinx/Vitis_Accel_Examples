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
    OpenCL Dataflow Example using xcl_dataflow attribute
    This is example of vector addition to demonstrate OpenCL Dataflow
xcl_dataflow
    functionality to perform task/subfunction level parallelism using
xcl_dataflow
    attribute. OpenCL xcl_dataflow instruct compiler to run subfunctions inside
kernel
    concurrently. In this Example, the kernel calls run_subfunc API to perform
vector
    addition implementation and which inturn is divided into three sub-function
APIs
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

// Includes
// Read Data from Global Memory and write into buffer_in
static void read_input(__global int* in, int* buffer_in, int size) {
    __attribute__((xcl_pipeline_loop(1))) __attribute__((xcl_loop_tripcount(c_size, c_size))) read
        : for (int i = 0; i < size; i++) {
        buffer_in[i] = in[i];
    }
}

// Read Input data from buffer_in and write the result into buffer_out
static void compute_add(int* buffer_in, int* buffer_out, int inc, int size) {
    __attribute__((xcl_pipeline_loop(1))) __attribute__((xcl_loop_tripcount(c_size, c_size))) compute
        : for (int i = 0; i < size; i++) {
        buffer_out[i] = buffer_in[i] + inc;
    }
}

// Read result from buffer_out and write the result to Global Memory
static void write_result(__global int* out, int* buffer_out, int size) {
    __attribute__((xcl_pipeline_loop(1))) __attribute__((xcl_loop_tripcount(c_size, c_size))) write
        : for (int i = 0; i < size; i++) {
        out[i] = buffer_out[i];
    }
}

// Dataflow attribute instructs compiler to run following three
// sub-functions concurrently
__attribute__((xcl_dataflow)) void run_subfunc(__global int* in, __global int* out, int inc, int size) {
    int buffer_in[BUFFER_SIZE];
    int buffer_out[BUFFER_SIZE];

    read_input(in, buffer_in, size);
    compute_add(buffer_in, buffer_out, inc, size);
    write_result(out, buffer_out, size);
}

/*
    Vector Addition Kernel Implementation using dataflow in sub functions
    Arguments:
        in   (input)  --> Input Vector
        out  (output) --> Output Vector
        inc  (input)  --> Increment
        size (input)  --> Size of Vector in Integer
*/
__kernel __attribute__((reqd_work_group_size(1, 1, 1))) void adder(__global int* in,
                                                                   __global int* out,
                                                                   int inc,
                                                                   int size) {
    run_subfunc(in, out, inc, size);
}
