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
    In this Example, a vector addition implementation is done
*******************************************************************************/

// Includes
#include <ap_int.h>

#define DATA_SIZE 4096

extern "C" {
/*
    Vector Addition Kernel Implementation using dataflow
    Arguments:
        in   (input)  --> Input Vector
        out  (output) --> Output Vector
        inc  (input)  --> Increment
        size (input)  --> Size of Vector in Integer
   */
void adder(unsigned int* in, unsigned int* out, int inc, int size) {
#pragma HLS INTERFACE m_axi port = in offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = out offset = slave bundle = gmem
#pragma HLS INTERFACE s_axilite port = in
#pragma HLS INTERFACE s_axilite port = out
#pragma HLS INTERFACE s_axilite port = inc
#pragma HLS INTERFACE s_axilite port = size
#pragma HLS INTERFACE s_axilite port = return
// Auto-pipeline is going to apply pipeline to this loop
vadd:
    for (int i = 0; i < size; i++) {
        out[i] = in[i] + inc;
    }
}
}
