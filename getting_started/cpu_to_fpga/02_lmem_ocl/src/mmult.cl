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

//This function represents an OpenCL kernel. The kernel will be a call from
//host application. The pointers in kernel parameters with the global 
//keyword represents cl_mem objects on the FPGA DDR memory. Local memory is
//created to reduce number of memory accesses. 

#define MAX_SIZE 64

// Tripcount identifiers
__constant int c_size = MAX_SIZE;

kernel __attribute__((reqd_work_group_size(1, 1, 1)))
void mmult( __global int* in1,  //Read-only input matrix1
            __global int* in2,  //Read-only input matrix2
            __global int* out,  //Output matrix
            int dim             //One dimension of the matrix
          )
{
    //Local memory to store input matrices
    //Local memory is implemented as BRAM memory blocks
    //MAX_SIZE * MAX_SIZE buffer is created because the size
    //need to be known at compile time
    __local int local_in1[MAX_SIZE][MAX_SIZE];
    __local int local_in2[MAX_SIZE][MAX_SIZE];
    __local int local_out[MAX_SIZE][MAX_SIZE];

    //Read the input data from DDR memory to local memory
    __attribute__((xcl_loop_tripcount(c_size*c_size, c_size*c_size)))
    read_in1: for(int iter = 0, i = 0, j = 0; iter < dim * dim; iter++, j++){
        if(j == dim){ j = 0; i++; }
        local_in1[i][j] = in1[iter];
        local_in2[i][j] = in2[iter];
    }
    
    //Reads the input_data from local memory, performs the computations 
    //and writes the data to local memory
    __attribute__((xcl_loop_tripcount(c_size, c_size)))
    for(int i = 0; i < dim; i++){
        __attribute__((xcl_loop_tripcount(c_size, c_size)))
        for(int j = 0; j < dim; j++){
            local_out[i][j] = 0;
            __attribute__((xcl_loop_tripcount(c_size, c_size)))
            write_data: for(int k = 0; k < dim; k++){
                local_out[i][j] += local_in1[i][k] * local_in2[k][ j];
            }
        }
    }    

    //Write the data from local memory to DDR memory
    __attribute__((xcl_loop_tripcount(c_size*c_size, c_size*c_size)))
    write_out: for(int iter = 0, i = 0, j = 0; iter < dim * dim; iter++, j++){
        if(j == dim){ j = 0; i++; }
        out[iter] = local_out[i][j];
    }
}
