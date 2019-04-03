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

SDx Key Concept :
    
    Split a complex kernel into multiple sub functions using OpenCL (Data
    flow) to build a highly parallel and area efficient kernel which also
    achieves higher clock frequency.

*******************************************************************************/

/*

Kernel Description (Bad Example) :
   
    This is a image processing based application which is mainly designed to showcase
    effectiveness of splitting a complex kernel into multiple sub functions using 
    streams to stream the data between kernels. 
    
    Note :

        Kernel in this file corresponds to bad design where complex operations 
        are present in a single kernel. Major drawback with this design is it lacks
        area efficiency (requires large buffers) and results in inefficient kernel
        design.
        
        1. The optimized version (good example) is present in the file sketch.cpp
        2. The optimized version uses streams to communicate and process efficiently.

    Kernels :
        
        This kernel applies the filters boost, median, sketch and flip on given image.
        
        Inputs/Outputs : 
        
            int *input  ( input)    --> Input image
            int *output (output)    --> Output image
            int  width  ( input)    --> Image width  (max 640)
            int  height ( input)    --> Image height (max 480)
*/

#include "defns.h"

// Get a 3x3 window based on the current position (row, col)
// The function is inlined, if the parent loop is pipelined,
// the loop fillWindow is automatically unrolled.
void getWindow(int *image, uint rgbWindow[SIZE], int row, int col, int width, int height)
{
#pragma HLS INLINE

    // Holds the rows/lines from which the data is to be picked
    int lineAddr[WINDOW];
    #pragma HLS ARRAY_PARTITION variable=lineAddr complete dim=1
    
    lineAddr[0] = row - 1;
    lineAddr[1] = row;
    lineAddr[2] = row + 1;
    
    // Handle Boundary Conditions
    if(row == 0){
        lineAddr[0] = 0;
    }
    else if(row == height-1) {
        lineAddr[0] = row-2;
        lineAddr[1] = row-1;
        lineAddr[2] = row;
    }
    
    // The fillWindow loop is unrolled automatically because it is
    // inlined into a loop that is pipelined.
    fillWindow: for(int i = 0; i < WINDOW; i++){
        rgbWindow[i*WINDOW + 0] = (col == 0)? image[lineAddr[i]*width + col] : image[lineAddr[i]*width + col-1];
        rgbWindow[i*WINDOW + 1] = image[lineAddr[i]*width + col];
        rgbWindow[i*WINDOW + 2] = (col == width-1)? image[lineAddr[i]*width + col] : image[lineAddr[i]*width + col+1];
    }
}

extern "C"
{
    void sketch_BAD( int *input,     // Input image
                        int *output,    // Output image
                        int width,      // Image width  (max 640)
                        int height      // Image height (max 480)
                    ) 
    {
    #pragma HLS INTERFACE m_axi port=input offset=slave bundle=gmem
    #pragma HLS INTERFACE m_axi port=output offset=slave bundle=gmem

    #pragma HLS INTERFACE s_axilite port=input bundle=control
    #pragma HLS INTERFACE s_axilite port=output bundle=control
    #pragma HLS INTERFACE s_axilite port=width bundle=control
    #pragma HLS INTERFACE s_axilite port=height bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control
    
        //Local Buffers to store output of boost, median and sketch filter
        int image[MAX_WIDTH * MAX_HEIGHT];
        int temp_res[MAX_WIDTH * MAX_HEIGHT]; //Hold boost and sketch outputs
        int med_out[MAX_WIDTH * MAX_HEIGHT];
        
        //Hold the window for processing
        uint rgbWindow[SIZE];
        #pragma HLS ARRAY_PARTITION variable=rgbWindow complete dim=1
        
        readImg: for(int i = 0; i < width*height; i++){
        #pragma HLS LOOP_TRIPCOUNT min=c_width*c_height max=c_width*c_height 
        #pragma HLS PIPELINE II=1
            image[i] = input[i];
        }
        
        // Boost Stage
        // Do boost filter on the image and write the output to temp_res
        boostHeight: for(int row = 0; row < height; row++){
        #pragma HLS LOOP_TRIPCOUNT min=height max=height
            boostWidth: for(int col = 0; col < width; col++){
            #pragma HLS LOOP_TRIPCOUNT min=c_width max=c_width
            #pragma HLS PIPELINE
                getWindow(image, rgbWindow, row, col, width, height);   // Get pixels within 3x3 aperture
                
                // Boost Value of the 3x3 rgbWindow
                // getBoost() is defined in kernels/boost_helper.h   
                temp_res[row*width + col] = getBoost(rgbWindow);
            }
        }
        
        // Median Stage
        // Do median filter on the image and write output to med_out
        medianHeight: for(int row = 0; row < height; row++){
        #pragma HLS LOOP_TRIPCOUNT min=c_height max=c_height
            medianWidth: for(int col = 0; col < width; col++){
            #pragma HLS LOOP_TRIPCOUNT min=c_width max=c_width
            #pragma HLS PIPELINE
                getWindow(image, rgbWindow, row, col, width, height);   // Get pixels within 3x3 aperture
                
                // Median of the 3x3 rgbWindow
                // getMedian() is defined in kernels/median_helper.h
                med_out[row*width + col] = getMedian(rgbWindow);                     
            }
        }
        
        // Sketch Stage
        // Do sketch operation on boost and median outputs.
        // Boost outputs are present in temp_res
        // Write the result back into temp_res.
        sketchLoop: for(int i = 0; i < width*height; i++) {
        #pragma HLS LOOP_TRIPCOUNT min=c_width*c_height max=c_width*c_height
        #pragma HLS PIPELINE II=1
            // Sketch operation on the current pixel of boost and median outputs
            // Boost operation output is in temp_res and
            // Median operation output is in med_out.
            // getSketch() is defined in kernels/sketch_helper.h
            temp_res[i] = getSketch(temp_res[i], med_out[i]);           
        }
        
        // Flips the Image by Reading Output Results from Sketch Output 
        // Burst write back results onto output
        flipOutput1: for(int row = 0 ; row < height ; row++){
        #pragma HLS LOOP_TRIPCOUNT min=c_height max=c_height
            // Reads from temp_res and flip the row and burst write output
            flipOutput2: for(int col = 0; col < width; col++){
            #pragma HLS LOOP_TRIPCOUNT min=c_width max=c_width
            #pragma HLS PIPELINE II=1
                output[row*width + col] = temp_res[row*width + width-col-1];
            }
        }
    }
}
