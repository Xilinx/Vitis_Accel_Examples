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
 This a kernel design of performing both vector addition and vector
multiplication
 on input vectors.

*******************************************************************************/

#include "krnl_vaddmul.h"

extern "C" {
void krnl_vaddmul(const v_dt* in1,             // Read-Only Vector 1
                  const v_dt* in2,             // Read-Only Vector 2
                  v_dt* out_add,               // Output Result for ADD
                  v_dt* out_mul,               // Output Result for MUL
                  const unsigned int size,     // Size in integer
                  const unsigned int num_times // Running the same kernel operations num_times
                  ) {
#pragma HLS INTERFACE m_axi port = in1 offset = slave bundle = gmem0
#pragma HLS INTERFACE m_axi port = in2 offset = slave bundle = gmem1
#pragma HLS INTERFACE m_axi port = out_add offset = slave bundle = gmem2
#pragma HLS INTERFACE m_axi port = out_mul offset = slave bundle = gmem3

#pragma HLS INTERFACE s_axilite port = in1
#pragma HLS INTERFACE s_axilite port = in2
#pragma HLS INTERFACE s_axilite port = out_add
#pragma HLS INTERFACE s_axilite port = out_mul

#pragma HLS INTERFACE s_axilite port = size
#pragma HLS INTERFACE s_axilite port = num_times
#pragma HLS INTERFACE s_axilite port = return

    unsigned int vSize = ((size - 1) / VDATA_SIZE) + 1;

    v_dt tmpIn1, tmpIn2;
    v_dt tmpOutAdd, tmpOutMul;

// Running same kernel operation num_times to keep the kernel busy for HBM
// bandwidth testing
L_vops:
    for (int count = 0; count < num_times; count++) {
    // Auto-pipeline is going to apply pipeline to this loop
    vops1:
        for (int i = 0; i < vSize; i++) {
            tmpIn1 = in1[i];
            tmpIn2 = in2[i];

        vops2:
            for (int k = 0; k < VDATA_SIZE; k++) {
                tmpOutAdd.data[k] = tmpIn1.data[k] + tmpIn2.data[k];
                tmpOutMul.data[k] = tmpIn1.data[k] * tmpIn2.data[k];
            }

            out_add[i] = tmpOutAdd;
            out_mul[i] = tmpOutMul;
        }
    }
}
}
