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

//This function represents an HLS kernel. The kernel will be a call from
//host application. The pointers in kernel parameters represents cl_mem
//objects on the FPGA DDR memory.
//Local memory is created to reduce number of memory accesses.

#define MAX_SIZE 64

// Tripcount identifiers
const int c_size = MAX_SIZE;

extern "C" {
void mmult(int *in1,   //Read-only input matrix1
           int *in2,   //Read-only input matrix2
           int *out_r, //Output matrix
           int dim     //One dimension of the matrix
) {
#pragma HLS INTERFACE m_axi port = in1 offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = in2 offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = out_r offset = slave bundle = gmem
#pragma HLS INTERFACE s_axilite port = in1 bundle = control
#pragma HLS INTERFACE s_axilite port = in2 bundle = control
#pragma HLS INTERFACE s_axilite port = out_r bundle = control
#pragma HLS INTERFACE s_axilite port = dim bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

    //Local memory to store input matrices
    //Local memory is implemented as BRAM memory blocks
    //MAX_SIZE * MAX_SIZE buffer is created because the size
    //need to be known at compile time
    int local_in1[MAX_SIZE][MAX_SIZE];
    int local_in2[MAX_SIZE][MAX_SIZE];
    int local_out[MAX_SIZE][MAX_SIZE];

//Read the input data from DDR memory to local memory
read_in:
    for (int iter = 0, i = 0, j = 0; iter < dim * dim; iter++, j++) {
       #pragma HLS LOOP_TRIPCOUNT min=c_size*c_size max=c_size*c_size
        if (j == dim) {
            j = 0;
            i++;
        }
        local_in1[i][j] = in1[iter];
        local_in2[i][j] = in2[iter];
    }

//Reads the input_data from local memory, performs the computations
//and writes the data to local memory
loop_1:
    for (int i = 0; i < dim; i++) {
       #pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
    loop_2:
        for (int j = 0; j < dim; j++) {
           #pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
            local_out[i][j] = 0;
        loop_3:
            for (int k = 0; k < dim; k++) {
               #pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
                local_out[i][j] += local_in1[i][k] * local_in2[k][j];
            }
        }
    }

//Write the data from local memory to DDR memory
write_out:
    for (int iter = 0, i = 0, j = 0; iter < dim * dim; iter++, j++) {
       #pragma HLS LOOP_TRIPCOUNT min=c_size*c_size max=c_size*c_size
        if (j == dim) {
            j = 0;
            i++;
        }
        out_r[iter] = local_out[i][j];
    }
}
}
