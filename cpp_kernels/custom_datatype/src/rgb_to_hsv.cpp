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
    This example demonstrate How a Custom data type can used in Kernel code.
    Here RGBcolor and HSVcolor two structure is declared and used as global
    memory access type.
*******************************************************************************/

#include "rgb_to_hsv.h"
extern "C" {

void rgb_to_hsv(RGBcolor* in,  // Access global memory as RGBcolor struct-wise
                HSVcolor* out, // Access Global Memory as HSVcolor struct-wise
                int size) {
    RGBcolor rgb;
    HSVcolor hsv;

// Single loop is sufficient to read each RGB pixel from Global memory,
// Converting RGB pixel to HSV Pixel, and writing HSV Pixel Back to Global
// Memory.
// Auto-pipeline is going to apply pipeline to this loop
rgb2hsv_loop:
    for (unsigned int i = 0; i < size; i++) {
// Loop is marked for pipeline. Compiler will be able to get Loop II=1
// as a result, Kernel will be able to do burst read and burst write.
// Kernel will be performing RGB to HSV conversion per pixel per clock.
#pragma HLS LOOP_TRIPCOUNT min = c_image_size max = c_image_size
        // LOOP TRIPCOUNT is added so that report estimate can provide estimated
        //  latency Information

        unsigned char rgbMin, rgbMax, tempS;

        rgb = in[i]; // Reading RGB Pixel from Global Memory

        // Getting Minimum and Maximum value in R, G, and B element of Pixel
        rgbMin = imin(rgb.r, (imin(rgb.g, rgb.b)));
        rgbMax = imax(rgb.r, (imax(rgb.g, rgb.b)));

        // Calculating TempS
        tempS = 255 * ((long)(rgbMax - rgbMin)) / rgbMax;

        // Algorithm to Calculate HSV from RSB
        if (rgbMax == 0) {
            hsv.h = 0;
            hsv.s = 0;
            hsv.v = 0;
        } else if (tempS == 0) {
            hsv.h = 0;
            hsv.s = 0;
            hsv.v = rgbMax;
        } else if (rgbMax == rgb.r) {
            hsv.h = 0 + 43 * (rgb.g - rgb.b) / (rgbMax - rgbMin);
            hsv.s = tempS;
            hsv.v = rgbMax;
        } else if (rgbMax == rgb.g) {
            hsv.h = 85 + 43 * (rgb.b - rgb.r) / (rgbMax - rgbMin);
            hsv.s = tempS;
            hsv.v = rgbMax;
        } else {
            hsv.h = 171 + 43 * (rgb.r - rgb.g) / (rgbMax - rgbMin);
            hsv.s = tempS;
            hsv.v = rgbMax;
        }

        // Writing computed HSV Pixel value into Global Memory
        out[i] = hsv;
    }
}
}
