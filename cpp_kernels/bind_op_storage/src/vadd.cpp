/**
* Copyright (C) 2019-2021 Xilinx, Inc
*
* Licensed under the Apache License, Version 2.0 (the "License"). You may
* not use this file except in compliance with the License. A copy of the
* License is located at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
* License for the specific language governing permissions and limitations
* under the License.
*/

#define BUFFER_SIZE 1024
#define DATA_SIZE 4096

// TRIPCOUNT identifier
const unsigned int c_len = DATA_SIZE / BUFFER_SIZE;
const unsigned int c_size = BUFFER_SIZE;

extern "C" {
void vadd(const unsigned int* in1, // Read-Only Vector 1
          const unsigned int* in2, // Read-Only Vector 2
          unsigned int* out_r,     // Output Result
          int size                 // Size in integer
          ) {
    unsigned int v1_buffer[BUFFER_SIZE];   // Local memory to store vector1
    unsigned int v2_buffer[BUFFER_SIZE];   // Local memory to store vector2
    unsigned int vout_buffer[BUFFER_SIZE]; // Local Memory to store result

// Using the BIND_OP pragma the user can specify the operator, implementation
// and latency
#pragma HLS BIND_OP variable = v1_buffer op = mul impl = DSP latency = 2
#pragma HLS BIND_OP variable = v2_buffer op = mul impl = DSP latency = 2
#pragma HLS BIND_OP variable = vout_buffer op = add impl = DSP
// Using the BIND STORAGE the used can choose the type, resource and latency
#pragma HLS BIND_STORAGE variable = v1_buffer type = RAM_1P impl = BRAM latency = 2
#pragma HLS BIND_STORAGE variable = v2_buffer type = RAM_1P impl = LUTRAM latency = 2
#pragma HLS BIND_STORAGE variable = vout_buffer type = RAM_1P impl = URAM

    // Per iteration of this loop perform BUFFER_SIZE vector addition
    for (int i = 0; i < size; i += BUFFER_SIZE) {
#pragma HLS LOOP_TRIPCOUNT min = c_len max = c_len
        int chunk_size = BUFFER_SIZE;
        // boundary checks
        if ((i + BUFFER_SIZE) > size) chunk_size = size - i;

    // Auto-pipeline is going to apply pipeline to these loops
    read1:
        for (int j = 0; j < chunk_size; j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
            v1_buffer[j] = in1[i + j] * in1[i + j];
        }

    read2:
        for (int j = 0; j < chunk_size; j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
            v2_buffer[j] = in2[i + j] * in2[i + j];
        }

    vadd:
        for (int j = 0; j < chunk_size; j++) {
// As the outer loop is not a perfect loop
#pragma HLS loop_flatten off
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
