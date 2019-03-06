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
    
    Split a complex kernel into multiple sub functions using OpenCL (Dataflow)
    to build a highly parallel and area efficient kernel which also achieves
    higher clock frequency.

*******************************************************************************/

/*

Kernel Description (Bad Example) :
   
    This is a image processing based application which is mainly designed to showcase
    effectiveness of splitting a complex kernel into multiple sub functions
    using streams to stream the data between functions. It also showcases the
    concurrent execution of multiple functions within a kernel.  
    
    Note :

        Kernel in this file corresponds to bad design in which all the operations
        are present in a single kernel. Major drawback with this design is it lacks
        area efficiency (requires large buffers) and results in inefficient kernel
        design. 
        
        1. The optimized version (good example) is present in the file sketch.cl
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

// Tripcount identifiers
__constant int c_width = MAX_WIDTH;
__constant int c_height = MAX_HEIGHT;
__constant int c_window = WINDOW;

__kernel __attribute__ ((reqd_work_group_size(1, 1, 1)))
void sketch_BAD( const __global int *input,     // Input image
                          __global int *output,    // Output image
                                   int width,      // Image width  (max 640)
                                   int height      // Image height (max 480)
                ) 
{
    //Local Buffers to store output of boost, median and sketch filter
    int image[MAX_WIDTH * MAX_HEIGHT];
    int temp_res[MAX_WIDTH * MAX_HEIGHT]; //Hold boost and sketch outputs
    int med_out[MAX_WIDTH * MAX_HEIGHT];
    
    //Hold the window for processing
    uint rgbWindow[SIZE] __attribute__((xcl_array_partition(complete, 1)));
    
    // Holds the rows/lines from which the data is to be picked
    int lineAddr[WINDOW] __attribute__((xcl_array_partition(complete, 1)));
    
    __attribute__((xcl_pipeline_loop(1))) 
    __attribute__((xcl_loop_tripcount(c_width*c_height, c_width*c_height)))
    readImg: for(int i = 0; i < width*height; i++){
        image[i] = input[i];
    }
    
    // Boost Stage
    // Do boost filter on the image and write the output to temp_res
    __attribute__((xcl_loop_tripcount(c_height, c_height)))
    boostHeight: for(int row = 0; row < height; row++){
        __attribute__((xcl_pipeline_loop))
        __attribute__((xcl_loop_tripcount(c_width, c_width)))
        boostWidth: for(int col = 0; col < width; col++){
            
            // Get pixels within 3x3 aperture
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
            __attribute__((xcl_loop_tripcount(c_window, c_window)))
            fillWindowBoost: for(int i = 0; i < WINDOW; i++){
                rgbWindow[i*WINDOW + 0] = (col == 0)? image[lineAddr[i]*width + col] : image[lineAddr[i]*width + col-1];
                rgbWindow[i*WINDOW + 1] = image[lineAddr[i]*width + col];
                rgbWindow[i*WINDOW + 2] = (col == width-1)? image[lineAddr[i]*width + col] : image[lineAddr[i]*width + col+1];
            }
            
            // Boost Value of the 3x3 rgbWindow
            // getBoost() is defined in kernels/boost_helper.h   
            temp_res[row*width + col] = getBoost(rgbWindow);
        }
    }
    
    // Median Stage
    // Do median filter on the image and write output to med_out
    __attribute__((xcl_loop_tripcount(c_height, c_height)))
    medianHeight: for(int row = 0; row < height; row++){
        __attribute__((xcl_pipeline_loop)) 
        __attribute__((xcl_loop_tripcount(c_width, c_width)))
        medianWidth: for(int col = 0; col < width; col++){
            
            // Get pixels within 3x3 aperture
            
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
            __attribute__((xcl_loop_tripcount(c_window, c_window)))
            fillWindowMedian: for(int i = 0; i < WINDOW; i++){
                rgbWindow[i*WINDOW + 0] = (col == 0)? image[lineAddr[i]*width + col] : image[lineAddr[i]*width + col-1];
                rgbWindow[i*WINDOW + 1] = image[lineAddr[i]*width + col];
                rgbWindow[i*WINDOW + 2] = (col == width-1)? image[lineAddr[i]*width + col] : image[lineAddr[i]*width + col+1];
            }
            
            // Median of the 3x3 rgbWindow
            // getMedian() is defined in kernels/median_helper.h
            med_out[row*width + col] = getMedian(rgbWindow);                     
        }
    }
    
    // Sketch Stage
    // Do sketch operation on boost and median outputs.
    // Boost outputs are present in temp_res
    // Write the result back into temp_res.
    __attribute__((xcl_pipeline_loop(1))) 
    __attribute__((xcl_loop_tripcount(c_width*c_height, c_width*c_height)))
    sketchLoop: for(int i = 0; i < width*height; i++) {
        // Sketch operation on the current pixel of boost and median outputs
        // Boost operation output is in temp_res and
        // Median operation output is in med_out.
        // getSketch() is defined in kernels/sketch_helper.h
        temp_res[i] = getSketch(temp_res[i], med_out[i]);           
    }
    
    // Flips the Image by Reading Output Results from Sketch Output 
    // Burst write back results onto output
    __attribute__((xcl_loop_tripcount(c_height, c_height)))
    flipOutput1: for(int row = 0 ; row < height ; row++){
        // Reads from temp_res and flip the row and burst write output
        __attribute__((xcl_pipeline_loop(1))) 
        __attribute__((xcl_loop_tripcount(c_width, c_width)))
        flipOutput2: for(int col = 0; col < width; col++){
            output[row*width + col] = temp_res[row*width + width-col-1];
        }
    }
}
