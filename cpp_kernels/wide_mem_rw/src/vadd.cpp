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
    Wide Memory Access Example using ap_uint<Width> datatype
    Description: This is vector addition example to demonstrate Wide Memory 
    access of 512bit Datawidth using ap_uint<> datatype which is defined inside
    'ap_int.h' file.
*******************************************************************************/

//Including to use ap_uint<> datatype
#include <ap_int.h>

#define BUFFER_SIZE 128
#define DATAWIDTH 512
#define DATATYPE_SIZE 32
#define VECTOR_SIZE                                                            \
    (DATAWIDTH / DATATYPE_SIZE) // vector size is 16 (512/32 = 16)
typedef ap_uint<DATAWIDTH> uint512_dt;
typedef ap_uint<DATATYPE_SIZE> din_type;
typedef ap_uint<DATATYPE_SIZE + 1> dout_type;

//TRIPCOUNT identifier
const unsigned int c_chunk_sz = BUFFER_SIZE;
const unsigned int c_size = VECTOR_SIZE;

/*
    Vector Addition Kernel Implementation using uint512_dt datatype 
    Arguments:
        in1   (input)     --> Input Vector1
        in2   (input)     --> Input Vector2
        out   (output)    --> Output Vector
        size  (input)     --> Size of Vector in Integer
   */
extern "C" {
void vadd(const uint512_dt *in1, // Read-Only Vector 1
          const uint512_dt *in2, // Read-Only Vector 2
          uint512_dt *out,       // Output Result
          int size               // Size in integer
) {
#pragma HLS INTERFACE m_axi port = in1 offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = in2 offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = out offset = slave bundle = gmem
#pragma HLS INTERFACE s_axilite port = in1 bundle = control
#pragma HLS INTERFACE s_axilite port = in2 bundle = control
#pragma HLS INTERFACE s_axilite port = out bundle = control
#pragma HLS INTERFACE s_axilite port = size bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control

    uint512_dt v1_local[BUFFER_SIZE];     // Local memory to store vector1
    uint512_dt result_local[BUFFER_SIZE]; // Local Memory to store result

    // Input vector size for interger vectors. However kernel is directly
    // accessing 512bit data (total 16 elements). So total number of read
    // from global memory is calculated here:
    int size_in16 = (size - 1) / VECTOR_SIZE + 1;

    //Per iteration of this loop perform BUFFER_SIZE vector addition
    for (int i = 0; i < size_in16; i += BUFFER_SIZE) {
       #pragma HLS LOOP_TRIPCOUNT min=c_chunk_sz/c_size max=c_chunk_sz/c_size
        int chunk_size = BUFFER_SIZE;

        //boundary checks
        if ((i + BUFFER_SIZE) > size_in16)
            chunk_size = size_in16 - i;

    //burst read first vector from global memory to local memory
    v1_rd:
        for (int j = 0; j < chunk_size; j++) {
           #pragma HLS PIPELINE II=1
           #pragma HLS LOOP_TRIPCOUNT min=c_chunk_sz max=c_chunk_sz
            v1_local[j] = in1[i + j];
        }

    //burst read second vector and perform vector addition
    v2_rd_add:
        for (int j = 0; j < chunk_size; j++) {
           #pragma HLS PIPELINE II=1
           #pragma HLS LOOP_TRIPCOUNT min=c_chunk_sz max=c_chunk_sz
            uint512_dt tmpV1 = v1_local[j];
            uint512_dt tmpV2 = in2[i + j];
            uint512_dt tmpOut = 0;
            din_type val1, val2;
            dout_type res;
        v2_parallel_add:
            for (int i = 0; i < VECTOR_SIZE; i++) {
               #pragma HLS UNROLL
                val1 =
                    tmpV1.range(DATATYPE_SIZE * (i + 1) - 1, i * DATATYPE_SIZE);
                val2 =
                    tmpV2.range(DATATYPE_SIZE * (i + 1) - 1, i * DATATYPE_SIZE);
                res = val1 + val2; // Vector Addition
                tmpOut.range(DATATYPE_SIZE * (i + 1) - 1, i * DATATYPE_SIZE) =
                    res; // Strobe Writing
            }
            result_local[j] = tmpOut;
        }

    //burst write the result
    out_write:
        for (int j = 0; j < chunk_size; j++) {
           #pragma HLS PIPELINE II=1
           #pragma HLS LOOP_TRIPCOUNT min=c_chunk_sz max=c_chunk_sz
            out[i + j] = result_local[j];
        }
    }
}
}
