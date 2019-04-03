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
    OpenCL Pipe Memory Example with Blocking Read/Write Operation
    Description: This vector addition example demonstrates OpenCL Pipe Memory 
    Usage. OpenCL PIPE memory functionality allows user to perform kernel to 
    kernel data transfer without using global memory. User can divide its 
    implementation into multiple kernels and can run all kernels concurrently.
    This example demonstrates the same. Here a vector addition implementation is 
    divided into three separate kernel units as below:

    1) Input Stage Kernel: 
        This Kernel reads the input vector from Global Memory and writes it into 
        Pipe Memory P0 using blocking write command.

    2) Adder Stage Kernel: 
        This Kernel reads the input vector from Pipe Memory P0 using blocking
        read command and increment the value by user specified value. It writes 
        the results into Pipe Memory P1 using blocking write command.

    3) Output Stage Kernel: 
        This Kernel reads the result vector from Pipe Memory P1 using blocking 
        read command and write the result into Global Memory Location.

    PIPE Memory based Adder will be implemented as below:
                     _____________
                    |             |<----- Input Vector from Global Memory
                    |  read_input |       __
                    |_____________|----->|  |
                     _____________       |  | p0
                    |             |<-----|__|
                    | compute_add |       __
                    |_____________|----->|  |
                     ______________      |  | p1
                    |              |<----|__|
                    | write_result |       
                    |______________|-----> Output result to Global Memory


*******************************************************************************/

//Declaring PIPE memory with Depth 32
pipe int p0 __attribute__((xcl_reqd_pipe_depth(32)));
pipe int p1 __attribute__((xcl_reqd_pipe_depth(32)));
//  In Kernel code, xcl_reqd_pipe_depth attribute is used to define the PIPE 
//  Memory depth to 32. Depth 32 means that PIPE memory can hold maximum 32 
//  elements at a given time. If PIPE memory is full, any blocking write command 
//  will go into wait state until some other kernel reads element from PIPE 
//  memory. Similarly if PIPE memory is empty (no element in memory), any 
//  blocking read command on this memory will go into wait state until some 
//  other kernel writes elements to PIPE Memory.
//  This blocking read and write functionality allow designer to synchronize the 
//  data across multiple kernels

// Tripcount identifiers
__constant int c_min_size = 1024;
__constant int c_max_size = 1024*1024;

// Input Stage Kernel : Read Data from Global Memory and write into Pipe P0
kernel __attribute__ ((reqd_work_group_size(1, 1, 1)))
void input_stage(__global int *input, int size)
{
    __attribute__((xcl_pipeline_loop(1))) 
    __attribute__((xcl_loop_tripcount(c_min_size, c_max_size)))
    mem_rd: for (int i = 0 ; i < size ; i++)
    {
        //blocking Write command to pipe P0
        write_pipe_block(p0, &input[i]);
    }
}

// Adder Stage Kernel: Read Input data from Pipe P0 and write the result 
// into Pipe P1
kernel __attribute__ ((reqd_work_group_size(1, 1, 1)))
void adder_stage(int inc, int size)
{
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_min_size, c_max_size)))
    execute: for(int i = 0 ; i < size ;  i++)
    {
        int input_data, output_data;
        //blocking read command to Pipe P0
        read_pipe_block(p0, &input_data);
        output_data = input_data + inc;
        //blocking write command to Pipe P1
        write_pipe_block(p1, &output_data);
    }
}


// Output Stage Kernel: Read result from Pipe P1 and write the result to Global
// Memory
kernel __attribute__ ((reqd_work_group_size(1, 1, 1)))
void output_stage(__global int *output, int size)
{
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_min_size, c_max_size)))
    mem_wr: for (int i = 0 ; i < size ; i++)
    {
        //blocking read command to Pipe P1
        read_pipe_block(p1, &output[i]);
    }
}
