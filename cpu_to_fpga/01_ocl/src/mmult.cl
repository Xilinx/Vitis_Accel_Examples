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
//keyword represents cl_mem objects on the FPGA DDR memory.

//Array Size to access 
#define DATA_SIZE 64

// Tripcount identifiers
__constant int c_size = DATA_SIZE;

kernel __attribute__((reqd_work_group_size(1, 1, 1)))
void mmult( __global int* in1,  //Read-only input matrix1
            __global int* in2,  //Read-only input matrix2
            __global int* out,  //Output matrix
            int dim             //One dimension of the matrix
          )        
{
    //Reads the data from DDR, performs the computation
    //and writes back the result to DDR.
    __attribute__((xcl_loop_tripcount(c_size, c_size)))
    for (int i = 0 ; i < dim ; i++){
        __attribute__((xcl_loop_tripcount(c_size, c_size)))
        for(int j = 0; j < dim; j++){
            out[i * dim + j] = 0;
            __attribute__((xcl_loop_tripcount(c_size, c_size)))
            for(int k = 0; k < dim; k++){
                out[i * dim + j] += in1[i * dim + k] * in2[k * dim + j];
            }
        }
    }
}
