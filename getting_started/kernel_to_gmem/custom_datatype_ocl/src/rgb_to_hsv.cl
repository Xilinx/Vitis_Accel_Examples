/**********
Copyright (c) 2018, Xilinx, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/

/*******************************************************************************
Description: 
    This example demonstrate How a Custom data type can used in Kernel code.  
    Here RGBcolor and HSVcolor two structure is declared and used as global 
    memory access type
*******************************************************************************/


#include "rgb_to_hsv.h"
/*
    RGB to HSV Conversion Kernel Implementation using uint16 datatype 
    Arguments:
        in   (input)     --> Input RGB Image
        out  (output)    --> Output HSV Image
        size  (input)     --> Size of Image
   */
kernel __attribute__ ((reqd_work_group_size(1, 1, 1)))
void rgb_to_hsv(
        const __global RGBcolor* in, // Access global Memory as RGBcolor struct-wise
        __global HSVcolor* out,      // Access global Memory as HSVcolor struct-wise
        uint size)
{
    RGBcolor rgb;
    HSVcolor hsv;

    //Single loop is sufficient to read each RGB pixel from Global memory, 
    //Converting RGB pixel to HSV Pixel, and writing HSV Pixel Back to Global
    //Memory.
     __attribute__((xcl_pipeline_loop))
    rgb2hsv_loop: for(uint i = 0 ; i < size ; i ++){
        // Loop is marked for pipeline. Compiler will be able to get Loop II=1 
        // as a result, Kernel will be able to do burst read and burst write. 
        // Kernel will be performing RGB to HSV conversion per pixel per clock. 
        
        uchar rgbMin, rgbMax, tempS;

        rgb =  in[i]; //Reading RGB Pixel from Global Memory


        //Getting Minimum and Maximum value in R, G, and B element of Pixel
        rgbMin = imin(rgb.r, (imin(rgb.g,rgb.b)));
        rgbMax = imax(rgb.r, (imax(rgb.g,rgb.b)));

        //Calculating TempS
        tempS = 255 * ((uint)(rgbMax - rgbMin)) / rgbMax;

        //Algorithm to Calculate HSV from RSB
        if (rgbMax == 0){
            hsv.h = 0;
            hsv.s = 0;
            hsv.v = 0;
        }else if (tempS == 0){
            hsv.h = 0;
            hsv.s = 0;
            hsv.v = rgbMax;
        }else if (rgbMax == rgb.r){
            hsv.h = 0 + 43 * (rgb.g - rgb.b) / (rgbMax - rgbMin);
            hsv.s = tempS;
            hsv.v = rgbMax;
        }else if (rgbMax == rgb.g){
            hsv.h = 85 + 43 * (rgb.b - rgb.r) / (rgbMax - rgbMin);
            hsv.s = tempS;
            hsv.v = rgbMax;
        }else{
            hsv.h = 171 + 43 * (rgb.r - rgb.g) / (rgbMax - rgbMin);
            hsv.s = tempS;
            hsv.v = rgbMax;
        }
        //Writing computed HSV Pixel value into Global Memory
        out[i] = hsv;
    }
}
