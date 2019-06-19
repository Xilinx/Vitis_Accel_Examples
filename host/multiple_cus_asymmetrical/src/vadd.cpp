/**********
Copyright (c) 2019, Xilinx, Inc.
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
// Work load of each CU
#define BUFFER_SIZE 4096

extern "C" {
void vadd(const int *in1, // Read-Only Vector 1
          const int *in2, // Read-Only Vector 2
          int *out,       // Output Result
          int size,       // Size of Vector Data
          int cu_id,      // CU ID
          int num_cu      // Number of CUs
) {
   #pragma HLS INTERFACE m_axi port=in1 offset=slave bundle=gmem
   #pragma HLS INTERFACE m_axi port=in2 offset=slave bundle=gmem
   #pragma HLS INTERFACE m_axi port=out offset=slave bundle=gmem

   #pragma HLS INTERFACE s_axilite port=in1 bundle=control
   #pragma HLS INTERFACE s_axilite port=in2 bundle=control
   #pragma HLS INTERFACE s_axilite port=out bundle=control
   #pragma HLS INTERFACE s_axilite port=size bundle=control
   #pragma HLS INTERFACE s_axilite port=cu_id bundle=control
   #pragma HLS INTERFACE s_axilite port=num_cu bundle=control
   #pragma HLS INTERFACE s_axilite port=return bundle=control

    // Local memory to store input and output vectors
    // Local memory is implemented as BRAM memory blocks
    // SDx assigns an independent local buffers for each compute unit
    // For example : if 8 compute units are used, each compute unit gets
    // three local buffers, a total 3*8 = 24 local buffers are allocated.
    int in1_lcl[BUFFER_SIZE];
    int in2_lcl[BUFFER_SIZE];
    int out_lcl[BUFFER_SIZE];

    int global_size = num_cu;
    // Computes vector addition operation iteratively over entire data set
    for (int offset = 0; offset < size; offset += BUFFER_SIZE * global_size) {
    // Enables burst reads on input vectors from global memory
    // Each Work_Item asynchronously moves its work load from global memory
    // to local memory (in1_lcl, in2_lcl) associated per each Work_Group

    // Burst read for in1_lcl
    readIn1:
        for (int j = 0; j < BUFFER_SIZE; j++) {
           #pragma HLS PIPELINE II=1
            in1_lcl[j] = in1[cu_id * BUFFER_SIZE + offset + j];
        }

    // Burst read for in2_lcl
    readIn2:
        for (int j = 0; j < BUFFER_SIZE; j++) {
           #pragma HLS PIPELINE II=1
            in2_lcl[j] = in2[cu_id * BUFFER_SIZE + offset + j];
        }

    // Performs vector addition.
    // Vector addition is operated on elements across multiple
    // compute units in parallel

    // This computation results in SIMD (Single Instruction Multiple
    // Data) type of execution over multiple CUs

    // Pipeline Operations
    vadd:
        for (int i = 0; i < BUFFER_SIZE; i++) {
           #pragma HLS PIPELINE II=1
            out_lcl[i] = in1_lcl[i] + in2_lcl[i];
        }

    // Burst write from kernel to global memory on "out" vector
    // Each Work_Item writes back results from local buffers to global memory
    // based on Work_Group which it belongs to.

    // Burst write from out_lcl
    writeOut:
        for (int j = 0; j < BUFFER_SIZE; j++) {
           #pragma HLS PIPELINE II=1
            out[cu_id * BUFFER_SIZE + offset + j] = out_lcl[j];
        }
    }
}
}