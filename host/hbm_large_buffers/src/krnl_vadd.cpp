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

/*******************************************************************************
Description:
    This is a simple vector addition example using C++ HLS.

*******************************************************************************/

#define VDATA_SIZE 16

// TRIPCOUNT indentifier
const unsigned int c_dt_size = VDATA_SIZE;

typedef struct v_datatype { unsigned int data[VDATA_SIZE]; } v_dt;

extern "C" {
void krnl_vadd(const v_dt* in1,                // Read-Only Vector 1
               const v_dt* in2,                // Read-Only Vector 2
               v_dt* out_r,                    // Output Result for Addition
               const unsigned int num_elements // Size in integer
               ) {
#pragma HLS INTERFACE m_axi port = in1 offset = slave bundle = gmem0
#pragma HLS INTERFACE m_axi port = in2 offset = slave bundle = gmem1
#pragma HLS INTERFACE m_axi port = out_r offset = slave bundle = gmem2

    unsigned int vSize = ((num_elements - 1) / VDATA_SIZE) + 1;

// Auto-pipeline is going to apply pipeline to this loop
vadd1:
    for (int i = 0; i < vSize; i++) {
    vadd2:
        for (int k = 0; k < VDATA_SIZE; k++) {
#pragma HLS LOOP_TRIPCOUNT min = c_dt_size max = c_dt_size
            out_r[i].data[k] = in1[i].data[k] + in2[i].data[k];
        }
    }
}
}
