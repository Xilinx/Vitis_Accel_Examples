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
    
    Split a complex kernel into multiple sub functions using OpenCL dataflow
    to build a highly parallel and area efficient kernel which also
    achieves higher clock frequency.

*******************************************************************************/

/*

Kernel Description (Good Example) :
   
    This is a image processing based application which is mainly designed to showcase
    effectiveness of splitting a complex function into multiple sub functions using 
    streams to stream the data between sub functions. It also showcases the concurrent
    execution of multiple functions.
    
    Kernels :
        
        The good kernel splits the operation into various stages and uses stream
        interface to communicate between the stages. This helps to avoid large
        intermediate buffers & communication between kernel to global memory
        frequently. It also helps in extracting more concurrency with the
        design.

        Note : 
            The default depth of the stream interface is 16 in this example.
            It is possible to set the stream depth by passing a compilation flag
            to xocc compiler

            Example : --xp "param:compiler.xclDataflowFifoDepth=16"
       
        1. Input
            
            This stage is to fetch data from DDR and fill the streams.
            This feeds input to the boost and median stages which operate concurrently.

        2. Boost
    
            This stage is to process the image in the form of 3x3 window.
            Maximum intensity value in a particular window is selected for output image.

        3. Median

            This stage is to process the image in the form of 3x3 window.
            Median intensity value in a particular window is selected for output image.

        4. Sketch

            This kernel subtracts results from Boost and Median stages. It also
            applies pixel intensity shift and equalization to achieve Sketch effect
            on the given input image.

        5. Output
            
            This stage flips the result from the sketch stage in horizontal axis and
            writes back the results the global memory.
        
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

// Fetch input image from global memory and write to boost_in & med_in streams.
// This stage supplies inputs to the boost and median stages.
void input_stage(__global int *input, int *boost_in, int *med_in, int size)
{
    // Burst Read on input and write to boost_in & med_in streams.
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_width*c_height, c_width*c_height)))
    readInput: for(int i = 0; i < size; i++){
        int in_lcl = input[i];
        boost_in[i] = in_lcl;
        med_in[i]   = in_lcl;
    }
}

// Boost filter
void boost_stage(int *boost_in, int *boost_out, int width, int height)
{
    // The line buffer holds the current rows of interest
    uint linebuf[BOOST_WINDOW+1][MAX_WIDTH]
    __attribute__((xcl_array_partition(complete, 1)));
    
    // Window to perform the calculation
    uint rgbWindow[SIZE] __attribute__((xcl_array_partition(complete,1)));
    
    // Row addresses of current window
    unsigned char rowAddr[BOOST_WINDOW + 1]
    __attribute__((xcl_array_partition(complete, 1)));
    
    // Fetch first lines 
    int initial_rows = 2;
    __attribute__((xcl_loop_tripcount(2,2)))
    fetchBoostLine1: for(int i = 0; i < initial_rows; i++){
        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_width, c_width)))
        fetchBoostLine2: for(int j = 0; j < width; j++){
            if(i == 0){
                linebuf[0][j] = boost_in[i * width + j];
                linebuf[1][j] = linebuf[0][j];
            }
            else{
                linebuf[2][j] = boost_in[i * width + j];
            }
        }
    }
    
    __attribute__((opencl_unroll_hint))
    __attribute__((xcl_loop_tripcount(c_window, c_window)))
    boostFillRow: for(int i = 0; i < BOOST_WINDOW+1; i++) {
        rowAddr[i] = i;
    }

    // Perform boost filtering over the current 3 lines for one entire row
    // getBoost() is defined in kernels/boost_helper.h  
    int count = 2;
    __attribute__((xcl_loop_tripcount(c_height, c_height)))
    boostHeight: for (int line = 0; line < height; line++){
        __attribute__((xcl_pipeline_loop))
        __attribute__((xcl_loop_tripcount(c_width, c_width)))
        boostWidth: for (int x=0; x < width; x++){
            
            // Get pixels within 3x3 aperture
            getBoostWindow(rgbWindow, linebuf, rowAddr, x, width);           
            
            // Write the result onto the pipe
            boost_out[line * width + x] = getBoost(rgbWindow);      
     
            // Fetch Next Row
            if(line < height-2) {
                linebuf[rowAddr[BOOST_WINDOW]][x] = boost_in[count * width + x];
                
                if(x == width-1) {
                    __attribute__((xcl_loop_tripcount(c_window, c_window)))
                    for(int row = 0; row < BOOST_WINDOW+1; row++)
                        rowAddr[row] = (rowAddr[row] + 1) % (BOOST_WINDOW + 1);
                }
            }
        }
        count++;
    }
}

// Median filter
void median_stage(int *med_in, int *med_out, int width, int height)
{

    // The line buffer holds the current rows of interest
    uint linebuf[MEDIAN_WINDOW+1][MAX_WIDTH]
    __attribute__((xcl_array_partition(complete, 1)));
    
    // Window to perform the calculation
    uint rgbWindow[SIZE] __attribute__((xcl_array_partition(complete,1)));
    
    // Row addresses of current window
    unsigned char rowAddr[MEDIAN_WINDOW + 1]
    __attribute__((xcl_array_partition(complete, 1)));
    
    // Fetch first lines
    int initial_rows = 2;
    __attribute__((xcl_loop_tripcount(2,2)))
    fetchMedianLine1: for(int i = 0; i < initial_rows; i++){
        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_width, c_width)))
        fetchMedianLine2: for(int j = 0; j < width; j++){
            if(i == 0){
                linebuf[0][j] = med_in[i * width + j];
                linebuf[1][j] = linebuf[0][j];
            }
            else{
                linebuf[2][j] = med_in[i * width + j];
            }
        }
    }
    
    __attribute__((opencl_unroll_hint))
    __attribute__((xcl_loop_tripcount(c_window, c_window)))
    medianFillRow: for(int i = 0; i < BOOST_WINDOW+1; i++) {
        rowAddr[i] = i;
    }

    // Perform median filtering over the current 3 lines for one entire row
    // getMedian() is defined in kernels/median_helper.h
    int count = 2;
    __attribute__((xcl_loop_tripcount(c_height, c_height)))
    medianHeight: for (int line = 0; line < height; line++){
        __attribute__((xcl_pipeline_loop))
        __attribute__((xcl_loop_tripcount(c_width, c_width)))
        medianWidth: for (int x=0; x < width; x++){
            
            // Get pixels within 3x3 aperture
            getMedianWindow(rgbWindow, linebuf, rowAddr, x, width);
            
            // Write the result onto the stream.
            med_out[line * width + x] = getMedian(rgbWindow);      

            // Fetch Next Row
            if(line < height-2) {
                linebuf[rowAddr[MEDIAN_WINDOW]][x] = med_in[count * width + x];
                if(x == width-1) {
                    medianUpdateRow: for(int row = 0; row < MEDIAN_WINDOW+1; row++)
                        rowAddr[row] = (rowAddr[row] + 1) % (BOOST_WINDOW + 1);
                }
            }
        }
        count++;
    }
}

// Sketch filter
void sketch_stage(int *boost_out, int *med_out, int *sketch_out, int size)
{
    uint boost_input, median_input;
    
    // Read inputs from the boost_out and med_out streams.
    // Do Sketch Operation and write into sketch_out stream.
    // getSketch() is defined in kernels/sketch_helper.h
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_width*c_height, c_width*c_height)))
    sketchLoop: for(int i = 0; i < size; i++){
        boost_input   = boost_out[i];
        median_input  = med_out[i];
        sketch_out[i] = getSketch(boost_input, median_input);
    }
}

void output_stage(__global int *output, int *sketch_out, int width, int height)
{
    // Holds Flipped Image Row
    uint result[MAX_WIDTH];
    int count = 0;
    // Flips the Image by Reading Output Results from Sketch Stream 
    // Burst write back results onto output
    __attribute__((xcl_loop_tripcount(c_height, c_height)))
    writeOutput1: for(int i = 0 ; i < height; i++){
        
        // Reads from sketch_out stream and flip the row
        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_width, c_width)))
        flipOutput: for(int j = width; j > 0; j--){
            result[j - 1] = sketch_out[i * width + count];
            count++;
        }

        count = 0;
        // Burst write output
        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_width, c_width)))
        writeOutput2: for(int k = 0; k < width; k++){
            output[i * width + k] = result[k];
        }
    }
}

__kernel 
__attribute__ ((reqd_work_group_size(1, 1, 1)))
__attribute__ ((xcl_dataflow))
void sketch_GOOD(__global int *input, __global int *output, int width, int height)
{

    // Input Image Streams for Boost and Median Kernels
    int boost_in[MAX_HEIGHT * MAX_WIDTH];
    int med_in[MAX_HEIGHT * MAX_WIDTH];

    // Output Streams for Boost, Median and Sketch
    int boost_out[MAX_HEIGHT * MAX_WIDTH];
    int med_out[MAX_HEIGHT * MAX_WIDTH];
    int sketch_out[MAX_HEIGHT * MAX_WIDTH];
    
    int size = width*height;
    
    input_stage(input, boost_in, med_in, size);
    boost_stage(boost_in, boost_out, width, height);
    median_stage(med_in, med_out, width, height);
    sketch_stage(boost_out, med_out, sketch_out, size);
    output_stage(output, sketch_out, width, height);
}
