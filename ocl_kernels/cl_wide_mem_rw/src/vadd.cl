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

/*******************************************************************************
Description:
    OpenCL Wide Memory Read/write Example
    Description: This is vector addition to demonstrate Wide Memory access of
    512bit Datawidth using uint16 openCL vector datatype.
*******************************************************************************/

#define DATA_SIZE 16384
#define LOCAL_MEM_SIZE 128
#define VECTOR_SIZE 16 //   using uint16 datatype so vector size is 16

// Tripcount identifiers
__constant int c_size = LOCAL_MEM_SIZE;
__constant int c_len = ((DATA_SIZE - 1) / VECTOR_SIZE + 1) / LOCAL_MEM_SIZE;

/*
    Vector Addition Kernel Implementation using uint16 datatype
    Arguments:
        in1   (input)     --> Input Vector1
        in2   (input)     --> Input Vector2
        out   (output)    --> Output Vector
        size  (input)     --> Size of Vector in Integer
   */
kernel __attribute__((reqd_work_group_size(1, 1, 1))) void vadd(const __global uint16* in1, // Read-Only Vector 1
                                                                const __global uint16* in2, // Read-Only Vector 2
                                                                __global uint16* out,       // Output Result
                                                                int size                    // Size in integer
                                                                ) {
    local uint16 v1_local[LOCAL_MEM_SIZE];     // Local memory to store vector1
    local uint16 result_local[LOCAL_MEM_SIZE]; // Local Memory to store result

    // Input vector size is in integer. It has to be changed into
    // Size of int16.
    int size_in16 = (size - 1) / VECTOR_SIZE + 1;

    // Per iteration of this loop perform LOCAL_MEM_SIZE vector addition
    __attribute__((xcl_loop_tripcount(c_len, c_len))) for (int i = 0; i < size_in16; i += LOCAL_MEM_SIZE) {
        int chunk_size = LOCAL_MEM_SIZE;

        // boundary checks
        if ((i + LOCAL_MEM_SIZE) > size_in16) chunk_size = size_in16 - i;

    // burst read first vector from global memory to local memory
    v1_rd:
        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_size, c_size))) for (int j = 0; j < chunk_size; j++) {
            v1_local[j] = in1[i + j];
        }

    // burst read second vector and perform vector addition
    v2_rd_add:
        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_size, c_size))) for (int j = 0; j < chunk_size; j++) {
            uint16 tmpV1 = v1_local[j];
            uint16 tmpV2 = in2[i + j];
            result_local[j] = tmpV1 + tmpV2; // Vector Addition Operation
        }

    // burst write the result
    out_wr:
        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_size, c_size))) for (int j = 0; j < chunk_size; j++) out[i + j] =
            result_local[j];
    }
}
