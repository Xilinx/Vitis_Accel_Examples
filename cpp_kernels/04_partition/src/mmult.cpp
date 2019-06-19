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

//This function represents an OpenCL kernel. The kernel will be call from
//host application. The pointers in kernel parameters with the global
//keyword represents cl_mem objects on the FPGA DDR memory. Array partitioning
//and loop unrolling is done to achieve better performance.

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
    //Complete partition done on dim 2 for in1, on dim 1 for in2 and on dim 2 for out_r
    int local_in1[MAX_SIZE][MAX_SIZE];
   #pragma HLS ARRAY_PARTITION variable=local_in1 dim=2 complete

    int local_in2[MAX_SIZE][MAX_SIZE];
   #pragma HLS ARRAY_PARTITION variable=local_in2 dim=1 complete

    int local_out[MAX_SIZE][MAX_SIZE];
#pragma HLS ARRAY_PARTITION variable = local_out dim = 2 complete

//Burst reads on input matrices from DDR memory
//Burst read for matrix local_in1 and local_in2
read_in1:
    for (int iter = 0, i = 0, j = 0; iter < dim * dim; iter++, j++) {
       #pragma HLS LOOP_TRIPCOUNT min=c_size*c_size max=c_size*c_size
       #pragma HLS PIPELINE II=1
        if (j == dim) {
            j = 0;
            i++;
        }
        local_in1[i][j] = in1[iter];
    }

read_in2:
    for (int iter = 0, i = 0, j = 0; iter < dim * dim; iter++, j++) {
       #pragma HLS LOOP_TRIPCOUNT min=c_size*c_size max=c_size*c_size
       #pragma HLS PIPELINE II=1
        if (j == dim) {
            j = 0;
            i++;
        }
        local_in2[i][j] = in2[iter];
    }

//Based on the functionality the number of iterations
//to be executed for "loop_3" must be "dim" size.
//But for the pipeline to happen in the "loop_2" the
//"loop_3" must be unrolled, to unroll the size cannot be dynamic.
//It gives better throughput with usage of additional resources.
loop_1:
    for (int i = 0; i < dim; i++) {
       #pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
    loop_2:
        for (int j = 0; j < dim; j++) {
           #pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
           #pragma HLS PIPELINE II=1
            local_out[i][j] = 0;
        loop_3:
            for (int k = 0; k < MAX_SIZE; k++) {
               #pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
               #pragma HLS UNROLL
                local_out[i][j] += local_in1[i][k] * local_in2[k][j];
            }
        }
    }

//Burst write from local_out to DDR memory
write_out:
    for (int iter = 0, i = 0, j = 0; iter < dim * dim; iter++, j++) {
       #pragma HLS LOOP_TRIPCOUNT min=c_size*c_size max=c_size*c_size
       #pragma HLS PIPELINE II=1
        if (j == dim) {
            j = 0;
            i++;
        }
        out_r[iter] = local_out[i][j];
    }
}
}
