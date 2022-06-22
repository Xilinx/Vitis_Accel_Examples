/**
* Copyright (C) 2022 Xilinx, Inc
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

#include "../include.h"
#include <adf.h>
#include <stdio.h>

// assuming NUM_CUS==2
void krnlmm(input_window_float* ina, input_window_float* inb, output_window_float* out) {
    float vala, valb;
    float valr;

    float matA[WIDTH];
    float matB[SIZE];

    for (unsigned ii = 0; ii < HEIGHT; ii++) {
        for (unsigned i = 0; i < WIDTH; i++) {
            window_readincr(inb, valb);
            matB[WIDTH * ii + i] = valb;
        }
    }

    for (unsigned ii = 0; ii < HEIGHT; ii++) {
        for (unsigned i = 0; i < WIDTH; i++) {
            window_readincr(ina, vala);
            matA[i] = vala;
        }
        // write back results
        for (unsigned i = 0; i < WIDTH; i++) {
            valr = 0.0;
            for (unsigned j = 0; j < WIDTH; j++) {
                valr += matA[j] * matB[i * WIDTH + j];
            }

            window_writeincr(out, valr);
        }
    }
}
