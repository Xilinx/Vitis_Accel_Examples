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
 
    OpenCL Kernel Example of Matrix Multiplication to demonstrate 2-D 
    full array access

*******************************************************************************/

// Define array size 
#define TEST_MATRIX_DIM 256

// Define max local buffer size
#define MAX_MATRIX_DIM 256

// Tripcount identifiers
__constant int c_size = TEST_MATRIX_DIM;

__kernel __attribute__ ((reqd_work_group_size(1, 1, 1)))
void mmult(__global int *a, __global int *b, __global int *c, int dim){

    // Define local buffers to hold global memory data
    local int bufa[MAX_MATRIX_DIM][MAX_MATRIX_DIM];
    local int bufb[MAX_MATRIX_DIM][MAX_MATRIX_DIM];
    local int bufc[MAX_MATRIX_DIM][MAX_MATRIX_DIM];

    // Size of the matrix
    int matrix_size = dim * dim;
  
    // Read data from global memory and write into local buffer
    int x = 0, y = 0;
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_size*c_size, c_size*c_size)))
    read_data_a: for (int i = 0 ; i < matrix_size ; i++){
        int tmpData_a = a[i];
        bufa[x][y] = tmpData_a;
        if (y == dim - 1){
            x++;
            y = 0;
        }else{
            y++;
        }
    }
  
    // Read data from global memory and write into local buffer
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_size*c_size, c_size*c_size)))
    read_data_b: for (int i = 0, x = 0, y = 0; i < matrix_size ; i++){
        int tmpData_b = b[i];
        bufb[x][y] = tmpData_b;
        if (y == dim - 1){
            x++;
            y = 0;
        }else{
            y++;
        }
    }
  
    // Calculate matrix multiplication using local data buffer based on 
    // input size and write results into local buffer
    __attribute__((xcl_loop_tripcount(c_size, c_size)))
    matrix_mult: for (int row = 0; row < dim; row++) {
        __attribute__((xcl_loop_tripcount(c_size, c_size)))
        for (int col = 0; col < dim; col++) {
            int result = 0;
            __attribute__((xcl_pipeline_loop(1)))
            __attribute__((xcl_loop_tripcount(c_size, c_size)))
            for (int k = 0; k < dim; k++) {
                result += bufa[row][k] * bufb[k][col];
            }
            bufc[row][col] = result;
        }
    }
  
    // Write results from local buffer to global memory
    int m = 0, n = 0;
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_size*c_size, c_size*c_size)))
    write_data: for (int i = 0 ; i < matrix_size ; i++){
        int tmpData_c = bufc[m][n];
        c[i] = tmpData_c;
        if (n == dim - 1){
            m++;
            n = 0;
        }else{
            n++;
        }
    }
}
