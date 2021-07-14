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

// This function represents an OpenCL kernel. The kernel will be call from
// host application using the xcl_run_kernels call. The pointers in kernel
// parameters with the global keyword represents cl_mem objects on the FPGA
// DDR memory.
//
#define BUFFER_SIZE 256
#define DATA_SIZE 1024

// TRIPCOUNT indentifier
__constant uint c_len = DATA_SIZE / BUFFER_SIZE;
__constant uint c_size = BUFFER_SIZE;

kernel __attribute__((reqd_work_group_size(1, 1, 1))) void vector_add(global int* c,
                                                                      global const int* a,
                                                                      global const int* b,
                                                                      const int n_elements) {
    int arrayA[BUFFER_SIZE];
    int arrayB[BUFFER_SIZE];

    __attribute__((xcl_loop_tripcount(c_len, c_len))) for (int i = 0; i < n_elements; i += BUFFER_SIZE) {
        int size = BUFFER_SIZE;

        if (i + size > n_elements) size = n_elements - i;

        __attribute__((xcl_loop_tripcount(c_size, c_size))) __attribute__((xcl_pipeline_loop(1))) readA
            : for (int j = 0; j < size; j++) {
            arrayA[j] = a[i + j];
        }

        __attribute__((xcl_loop_tripcount(c_size, c_size))) __attribute__((xcl_pipeline_loop(1))) readB
            : for (int j = 0; j < size; j++) {
            arrayB[j] = b[i + j];
        }

        __attribute__((xcl_loop_tripcount(c_size, c_size))) __attribute__((xcl_pipeline_loop(1))) vadd_writeC
            : for (int j = 0; j < size; j++) {
            c[i + j] = arrayA[j] + arrayB[j];
        }
    }
}
