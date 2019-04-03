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

SDx Key Concept :
    
    This is a matrix multiplication example which showcases the "Systolic Array"
    based algorithm design. Systolic array type of implementation is well suited
    for FPGAs.

*******************************************************************************/

/*

Kernel Description :
   
    This kernel is a systolic array based matrix multiplication. Though the 
    maximum size of the input matrices are restricted to a smaller MAX_SIZE, it
    is still possible to use this approach and get better performance for larger
    matrices by using tiling.
    
    Arguments :
    
        int *a     (input )  --> Input  Matrix A
        int *b     (input )  --> Input  Matrix B
        int *c     (output)  --> Output Matrix
        int  a_row (input )  --> Row Size Matrix A
        int  a_col (input )  --> Col Size Matrix A
        int  b_col (input )  --> Col Size Matrix B

    Kernel Configuration :
        
        Max Size    --> 12
    
    Note : 
        Max Size is dependent on the available DSP resources in the FPGA
*/

//Maximum Array Size
#define MAX_SIZE 12

// Tripcount identifiers
__constant int c_size = MAX_SIZE;

__kernel __attribute__((reqd_work_group_size(1, 1, 1)))
void mmult(
            __global int *a,    // Read-Only Matrix A
            __global int *b,    // Read-Only Matrix B
            __global int *c,    // Output Result
                  int a_row,    // Matrix A Row Size
                  int a_col,    // Matrix A Col Size
                  int b_col     // Matrix B Col Size
        )
{
    
    int b_row = a_col;
    int c_row = a_row;
    int c_col = b_col;
    
    // Local memory to store input and output matrices
    
    int localA[MAX_SIZE][MAX_SIZE]  __attribute__((xcl_array_partition(complete, 1)));;
    int localB[MAX_SIZE][MAX_SIZE]  __attribute__((xcl_array_partition(complete, 2)));;
    int localC[MAX_SIZE][MAX_SIZE]  __attribute__((xcl_array_partition(complete, 0)));;
    
    // Burst reads on input matrices from global memory
    // Read Input A
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_size*c_size, c_size*c_size)))
    readA: for(int loc = 0, i = 0, j = 0; loc < a_row*a_col; loc++, j++) {
        if(j == a_col) { i++; j = 0;}
        localA[i][j] = a[loc];
    }
    
    // Read Input B
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_size*c_size, c_size*c_size)))
    readB: for(int loc = 0, i = 0, j = 0; loc < b_row*b_col; loc++, j++) {
        if(j == b_col) { i++; j = 0; }
        localB[i][j] = b[loc];
    }
    
    // Perform systolic matrix multiply
    // local matrices localA and localB have been partitioned in dimensions 
    // 1 and 2 respectively. local matrix C has been partitioned completely
    
    // This partitioning enables to access MAX_SIZE elements in parallel in 
    // the local matrices. Because of the mode of access of array elements, 
    // we are able to perform MAX_SIZE*MAX_SIZE operations in parallel.
    
    // Note : i, j and k loops are interchanged.
    
    // The top loop systolic1 runs only for a_col iterations instead of 
    // MAX_SIZE like the inner loops. The inner loops have fixed loop
    // iteration counts to enable complete unroll
    
    // The following diagram explains how the matrix multiply happens
    // 
    //        B_0        B_1        B_2        B_3
    //         |          |          |          |
    //         v          v          v          v
    //        ___        ___        ___        ___
    //       |   |      |   |      |   |      |   |
    //  A0_->|C00| ---- |C01| ---- |C02| ---- |C03|
    //       |___|      |___|      |___|      |___|
    //         |          |          |          |
    //        ___        ___        ___        ___
    //       |   |      |   |      |   |      |   |
    //  A1_->|C10| ---- |C11| ---- |C12| ---- |C13|
    //       |___|      |___|      |___|      |___|
    //         |          |          |          |
    //        ___        ___        ___        ___
    //       |   |      |   |      |   |      |   |
    //  A2_->|C20| ---- |C21| ---- |C22| ---- |C23|
    //       |___|      |___|      |___|      |___|
    //         |          |          |          |
    //        ___        ___        ___        ___
    //       |   |      |   |      |   |      |   |
    //  A3_->|C30| ---- |C31| ---- |C32| ---- |C33|
    //       |___|      |___|      |___|      |___|
     
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_size, c_size)))
    systolic1: for(int k = 0; k < a_col; k++) {
        __attribute__((xcl_loop_tripcount(c_size, c_size)))
        systolic2: for(int i = 0; i < MAX_SIZE; i++) {
            __attribute__((xcl_loop_tripcount(c_size, c_size)))
            systolic3: for(int j = 0; j < MAX_SIZE; j++) {
                
                // Get previous sum
                int last = (k==0) ? 0 : localC[i][j];
                
                // Update current sum
                // Handle boundary conditions
                int a_val = (i < a_row && k < a_col)? localA[i][k] : 0;
                int b_val = (k < b_row && j < b_col)? localB[k][j] : 0;
                int result = last + a_val*b_val;
                
                // Write back results
                localC[i][j] = result;
            }
        }
    }
    
    // Burst write from output matrices to global memory
    // Burst write from matrix C
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_size*c_size, c_size*c_size)))
    writeC: for(int loc = 0, i = 0, j = 0; loc < c_row*c_col; loc++, j++) {
        if(j == c_col) { i++; j = 0; }
        c[loc] = localC[i][j];
    }
}
