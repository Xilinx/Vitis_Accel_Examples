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

    This is a simple example of vector addition. For hw_emu and hw flows, the
    same is done via an RTL Kernel.
    For sw_emu, the following C-Model is used. The main requirement here is
    that only the top-level function should have HLS-C like interface while, the
    internal implementation of the function does not need to be synthesizable
    HLS-C.

*******************************************************************************/

// Includes
#include "ap_int.h"

extern "C" {

/*
    Vector Addition Kernel

    Arguments:
        a  (input)  --> Input vector 1
        b  (input)  --> Input vector 2
        c  (output) --> Output vector
        length_r (input)  --> Number of elements in vector
*/

void krnl_vadd_rtl(uint32_t* a, uint32_t* b, uint32_t* c, ap_uint<32> length_r) {
#pragma HLS INTERFACE m_axi port = a bundle = gmem
#pragma HLS INTERFACE m_axi port = b bundle = gmem
#pragma HLS INTERFACE m_axi port = c bundle = gmem
#pragma HLS INTERFACE ap_ctrl_hs port = return

    for (int i = 0; i < length_r; i++) c[i] = a[i] + b[i];
}
}
