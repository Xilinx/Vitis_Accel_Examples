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
    OpenCL Kernel Example to demonstrate burst read and write access from
    Global Memory
*******************************************************************************/

// Below Macro should be commented for optimized design. Xilinx recommends to
// use for loop approach instead of async_work_grop_copy() API burst read/write
//#define USE_ASYNC_API

#define DATA_SIZE 2048
#define BURSTBUFFERSIZE 256

// Tripcount identifiers
__constant int c_size = BURSTBUFFERSIZE;
__constant int c_len = DATA_SIZE / BURSTBUFFERSIZE;

__kernel __attribute__((reqd_work_group_size(1, 1, 1))) void vadd(__global int* a, int size, int inc_value) {
    local int burstbuffer[BURSTBUFFERSIZE];
    // Per iteration of this loop perform BURSTBUFFERSIZE vector addition
    __attribute__((xcl_loop_tripcount(c_len, c_len))) for (int i = 0; i < size; i += BURSTBUFFERSIZE) {
        int chunk_size = BURSTBUFFERSIZE;
        // boundary checks
        if ((i + BURSTBUFFERSIZE) > size) chunk_size = size - i;

#ifndef USE_ASYNC_API // Burst Read using For loop (Xilinx recommended)
        // read data from global memory into local buffer, the sequential read in a
        // for loop can be inferred to a memory burst access
        __attribute__((xcl_pipeline_loop(1))) __attribute__((xcl_loop_tripcount(c_size, c_size))) read_buf
            : for (int j = 0; j < chunk_size; j++) {
            burstbuffer[j] = a[i + j];
        }
#else // Burst Read using async_work_group_copy() API (Not recommended)
        async_work_group_copy(burstbuffer, &a[i], chunk_size, 0);
// This code is kept here for intentionally to describe about
// async_work_group_copy() comparison with respect to for loop
// based implementation. async_work_group_copy() API is another way of
// implementing burst read from global memory,
// but it is not recommended to use async_work_group_copy() API. Most of time,
// tool generated pipeline design using this API
// will not as efficient as 'for' loop based implementation.
// async_work_group_copy() may creates pipeline design
// larger number of pipeline stages
#endif
        // calculate and write results to global memory, the sequential write in a
        // for loop can be inferred to a memory burst access
        __attribute__((xcl_pipeline_loop(1))) __attribute__((xcl_loop_tripcount(c_size, c_size))) calc_write
            : for (int j = 0; j < chunk_size; j++) {
            burstbuffer[j] = burstbuffer[j] + inc_value;
            a[i + j] = burstbuffer[j];
        }
    }
}
