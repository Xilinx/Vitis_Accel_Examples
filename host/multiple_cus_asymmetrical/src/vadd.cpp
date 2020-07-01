/**********
Copyright (c) 2020, Xilinx, Inc.
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
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/
// Work load of each CU
#define BUFFER_SIZE 1024
#define DATA_SIZE 4096

// TRIPCOUNT indentifier
const unsigned int c_len = DATA_SIZE / BUFFER_SIZE;
const unsigned int c_size = BUFFER_SIZE;

/*
    Vector Addition Kernel Implementation
    Arguments:
        in1   (input)     --> Input Vector1
        in2   (input)     --> Input Vector2
        out_r   (output)    --> Output Vector
        size  (input)     --> Size of Vector in Integer
*/

extern "C" {
void vadd(const unsigned int *in1, // Read-Only Vector 1
          const unsigned int *in2, // Read-Only Vector 2
          unsigned int *out_r,     // Output Result
          int size                 // Size in integer
          ) {

  unsigned int v1_buffer[BUFFER_SIZE];   // Local memory to store vector1
  unsigned int v2_buffer[BUFFER_SIZE];   // Local memory to store vector2
  unsigned int vout_buffer[BUFFER_SIZE]; // Local Memory to store result

  // Per iteration of this loop perform BUFFER_SIZE vector addition
  for (int i = 0; i < size; i += BUFFER_SIZE) {
#pragma HLS LOOP_TRIPCOUNT min = c_len max = c_len
    int chunk_size = BUFFER_SIZE;
    // boundary checks
    if ((i + BUFFER_SIZE) > size)
      chunk_size = size - i;

  // Transferring data in bursts hides the memory access latency as well as
  // improves bandwidth utilization and efficiency of the memory controller.
  // It is recommended to infer burst transfers from successive requests of data
  // from consecutive address locations.
  // A local memory vl_local is used for buffering the data from a single burst.
  // The entire input vector is read in multiple bursts.
  // The choice of LOCAL_MEM_SIZE depends on the specific applications and
  // available on-chip memory on target FPGA.
  // burst read of v1 and v2 vector from global memory

  // Auto-pipeline is going to apply pipeline to these loops
  read1:
    for (int j = 0; j < chunk_size; j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
      v1_buffer[j] = in1[i + j];
    }

  read2:
    for (int j = 0; j < chunk_size; j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
      v2_buffer[j] = in2[i + j];
    }

  // PIPELINE pragma reduces the initiation interval for loop by allowing the
  // concurrent executions of operations
  vadd:
    for (int j = 0; j < chunk_size; j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
      // perform vector addition
      vout_buffer[j] = v1_buffer[j] + v2_buffer[j];
    }

  // burst write the result
  write:
    for (int j = 0; j < chunk_size; j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
      out_r[i + j] = vout_buffer[j];
    }
  }
}
}
