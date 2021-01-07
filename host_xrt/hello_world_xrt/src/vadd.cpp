/**
* Copyright (C) 2020 Xilinx, Inc
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

/*
    Vector Addition Kernel Implementation
    Arguments:
        in1   (input)     --> Input Vector1
        in2   (input)     --> Input Vector2
        out_r   (output)    --> Output Vector
        size  (input)     --> Size of Vector in Integer
*/

extern "C" {
void vadd(const unsigned int* in1, // Read-Only Vector 1
          const unsigned int* in2, // Read-Only Vector 2
          unsigned int* out_r,     // Output Result
          int size                 // Size in integer
          ) {
    // In Vitis flow, a kernel can be defined without interface pragma. For such
    // case, it follows default behavioral.
    // All pointer arguments (in1,in2,out_r) will be referred as Memory Mapped
    // pointers using single M_AXI interface.
    // All the scalar arguments (size) and return argument will be associated to
    // single s_axilite interface.
    unsigned int v1_buffer[BUFFER_SIZE];   // Local memory to store vector1
    unsigned int v2_buffer[BUFFER_SIZE];   // Local memory to store vector2
    unsigned int vout_buffer[BUFFER_SIZE]; // Local Memory to store result

    // Per iteration of this loop perform BUFFER_SIZE vector addition
    for (int i = 0; i < size; i += BUFFER_SIZE) {
#pragma HLS LOOP_TRIPCOUNT min = c_len max = c_len
        int chunk_size = BUFFER_SIZE;
        // boundary checks
        if ((i + BUFFER_SIZE) > size) chunk_size = size - i;

    // Transferring data in bursts hides the memory access latency as well as
    // improves bandwidth utilization and efficiency of the memory controller.
    // It is recommended to infer burst transfers from successive requests of data
    // from consecutive address locations.
    // A local memory vl_local is used for buffering the data from a single burst.
    // The entire input vector is read in multiple bursts.
    // The choice of LOCAL_MEM_SIZE depends on the specific applications and
    // available on-chip memory on target FPGA.
    // burst read of v1 and v2 vector from global memory

    read1:
        for (int j = 0; j < chunk_size; j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
#pragma HLS PIPELINE II = 1
            v1_buffer[j] = in1[i + j];
        }

    read2:
        for (int j = 0; j < chunk_size; j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
#pragma HLS PIPELINE II = 1
            v2_buffer[j] = in2[i + j];
        }

    // PIPELINE pragma reduces the initiation interval for loop by allowing the
    // concurrent executions of operations
    vadd:
        for (int j = 0; j < chunk_size; j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
#pragma HLS PIPELINE II = 1
            // perform vector addition
            vout_buffer[j] = v1_buffer[j] + v2_buffer[j];
        }

    // burst write the result
    write:
        for (int j = 0; j < chunk_size; j++) {
#pragma HLS LOOP_TRIPCOUNT min = c_size max = c_size
#pragma HLS PIPELINE II = 1
            out_r[i + j] = vout_buffer[j];
        }
    }
}
}
