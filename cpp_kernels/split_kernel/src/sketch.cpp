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
    
    Split a complex kernel into multiple sub functions using HLS Streams (Data
    flow) to build a highly parallel and area efficient kernel which also
    achieves higher clock frequency.

*******************************************************************************/

/*

Kernel Description (Good Example) :
   
    This is a image processing based application which is mainly designed to showcase
    effectiveness of splitting a complex kernel into multiple sub functions using 
    streams to stream the data between sub functions. It also showcases the concurrent
    execution of multiple functions.
    

    Kernels :
        
        The good kernel splits the operation into various stages and uses hls::streams
        to communicate between the stages. This helps to avoid large intermediate
        buffers & communication between kernel to global memory frequently. It also
        helps in extracting more concurrency with the design.
        
       
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
#include <hls_stream.h>
#include <stdio.h>

// Fetch input image from global memory and write to boost_in & med_in streams.
// This stage supplies inputs to the boost and median stages.
void input_stage(int *input,
                 hls::stream<uint> &boost_in,
                 hls::stream<uint> &med_in,
                 int size) {
// Burst Read on input and write to boost_in & med_in streams.
readInput:
    for (int i = 0; i < size; i++) {
       #pragma HLS LOOP_TRIPCOUNT min=c_width*c_height max=c_width*c_height
       #pragma HLS PIPELINE II=1
        int in_lcl = input[i];
        boost_in << in_lcl;
        med_in << in_lcl;
    }
}

// Boost filter
void boost_stage(hls::stream<uint> &boost_in,
                 hls::stream<uint> &boost_out,
                 int width,
                 int height) {
    // The line buffer holds the current rows of interest
    uint linebuf[BOOST_WINDOW + 1][MAX_WIDTH];
   #pragma HLS ARRAY_PARTITION variable=linebuf complete dim=1

    // Window to perform the calculation
    uint rgbWindow[SIZE];
   #pragma HLS ARRAY_PARTITION variable=rgbWindow complete dim=1

    // Row addresses of current window
    unsigned char rowAddr[BOOST_WINDOW + 1];
#pragma HLS ARRAY_PARTITION variable = rowAddr complete dim = 1

// Fetch first lines
fetchBoostLine1:
    for (int i = 0; i < width; i++) {
       #pragma HLS LOOP_TRIPCOUNT min=c_width max=c_width
       #pragma HLS PIPELINE II=1
        boost_in >> linebuf[0][i];
        linebuf[1][i] = linebuf[0][i];
    }

fetchBoostLine2:
    for (int i = 0; i < width; i++) {
       #pragma HLS LOOP_TRIPCOUNT min=c_width max=c_width
       #pragma HLS PIPELINE II=1
        boost_in >> linebuf[2][i];
    }

boostFillRow:
    for (int i = 0; i < BOOST_WINDOW + 1; i++) {
       #pragma HLS UNROLL
        rowAddr[i] = i;
    }

// Perform boost filtering over the current 3 lines for one entire row
// getBoost() is defined in kernels/boost_helper.h
boostHeight:
    for (int line = 0; line < height; line++) {
       #pragma HLS LOOP_TRIPCOUNT min=c_height max=c_height
    boostWidth:
        for (int x = 0; x < width; x++) {
           #pragma HLS LOOP_TRIPCOUNT min=c_width max=c_width
           #pragma HLS PIPELINE

            // Get pixels within 3x3 aperture
            getBoostWindow(rgbWindow, linebuf, rowAddr, x, width);

            // Write the result onto the stream
            boost_out << getBoost(rgbWindow);

            // Fetch Next Row
            if (line < height - 2) {
                boost_in >> linebuf[rowAddr[BOOST_WINDOW]][x];

                if (x == width - 1) {
                boostUpdateRow:
                    for (int row = 0; row < BOOST_WINDOW + 1; row++)
                        rowAddr[row] = (rowAddr[row] + 1) % (BOOST_WINDOW + 1);
                }
            }
        }
    }
}

// Median filter
void median_stage(hls::stream<uint> &med_in,
                  hls::stream<uint> &med_out,
                  int width,
                  int height) {
    // The line buffer holds the current rows of interest
    uint linebuf[MEDIAN_WINDOW + 1][MAX_WIDTH];
   #pragma HLS ARRAY_PARTITION variable=linebuf complete dim=1

    // Window to perform the calculation
    uint rgbWindow[SIZE];
   #pragma HLS ARRAY_PARTITION variable=rgbWindow complete dim=1

    // Row addresses of current window
    unsigned char rowAddr[MEDIAN_WINDOW + 1];
#pragma HLS ARRAY_PARTITION variable = rowAddr complete dim = 1

// Fetch first lines
fetchMedianLine1:
    for (int i = 0; i < width; i++) {
       #pragma HLS LOOP_TRIPCOUNT min=c_width max=c_width
       #pragma HLS PIPELINE II=1
        med_in >> linebuf[0][i];
        linebuf[1][i] = linebuf[0][i];
    }

fetchMedianLine2:
    for (int i = 0; i < width; i++) {
       #pragma HLS LOOP_TRIPCOUNT min=c_width max=c_width
       #pragma HLS PIPELINE II=1
        med_in >> linebuf[2][i];
    }

medianFillRow:
    for (int i = 0; i < MEDIAN_WINDOW + 1; i++) {
       #pragma HLS UNROLL
        rowAddr[i] = i;
    }

// Perform median filtering over the current 3 lines for one entire row
// getMedian() is defined in kernels/median_helper.h
medianHeight:
    for (int line = 0; line < height; line++) {
       #pragma HLS LOOP_TRIPCOUNT min=c_height max=c_height
    medianWidth:
        for (int x = 0; x < width; x++) {
           #pragma HLS LOOP_TRIPCOUNT min=c_width max=c_width
           #pragma HLS PIPELINE

            // Get pixels within 3x3 aperture
            getMedianWindow(rgbWindow, linebuf, rowAddr, x, width);

            // Write the result onto the stream
            med_out << getMedian(rgbWindow);

            // Fetch Next Row
            if (line < height - 2) {
                med_in >> linebuf[rowAddr[MEDIAN_WINDOW]][x];
                if (x == width - 1) {
                medianUpdateRow:
                    for (int row = 0; row < MEDIAN_WINDOW + 1; row++)
                        rowAddr[row] = (rowAddr[row] + 1) % (BOOST_WINDOW + 1);
                }
            }
        }
    }
}

// Sketch filter
void sketch_stage(hls::stream<uint> &boost_out,
                  hls::stream<uint> &med_out,
                  hls::stream<uint> &sketch_out,
                  int size) {
    uint boost_input, median_input;

// Read inputs from the boost_out and med_out streams.
// Do Sketch Operation and write into sketch_out stream.
// getSketch() is defined in kernels/sketch_helper.h
sketchLoop:
    for (int i = 0; i < size; i++) {
       #pragma HLS LOOP_TRIPCOUNT min=c_width*c_height max=c_width*c_height
       #pragma HLS PIPELINE II=1
        boost_out >> boost_input;
        med_out >> median_input;
        sketch_out << getSketch(boost_input, median_input); // Sketch Operation
    }
}

void output_stage(int *output,
                  hls::stream<uint> &sketch_out,
                  int width,
                  int height) {
    // Holds Flipped Image Row
    uint result[MAX_WIDTH];

// Flips the Image by Reading Output Results from Sketch stream
// Burst write back results onto output
writeOutput1:
    for (int i = 0; i < height; i++) {
       #pragma HLS LOOP_TRIPCOUNT min=c_height max=c_height

    // Reads from sketch_out stream and flip the row
    flipOutput:
        for (int j = width; j > 0; j--) {
           #pragma HLS LOOP_TRIPCOUNT min=c_width max=c_width
           #pragma HLS PIPELINE II=1
            sketch_out >> result[j - 1];
        }

    // Burst write output
    writeOutput2:
        for (int k = 0; k < width; k++) {
           #pragma HLS LOOP_TRIPCOUNT min=c_width max=c_width
           #pragma HLS PIPELINE II=1
            output[i * width + k] = result[k];
        }
    }
}

extern "C" {
void sketch_GOOD(int *input, int *output, int width, int height) {
   #pragma HLS DATAFLOW

   #pragma HLS INTERFACE m_axi port=input offset=slave bundle=gmem
   #pragma HLS INTERFACE m_axi port=output offset=slave bundle=gmem

   #pragma HLS INTERFACE s_axilite port=input bundle=control
   #pragma HLS INTERFACE s_axilite port=output bundle=control
   #pragma HLS INTERFACE s_axilite port=width bundle=control
   #pragma HLS INTERFACE s_axilite port=height bundle=control
   #pragma HLS INTERFACE s_axilite port=return bundle=control

    // Input Image Streams for Boost and Median Kernels
    hls::stream<uint> boost_in("boost_inputstream"),
        med_in("median_inputstream");
   #pragma HLS STREAM variable=boost_in depth=16
   #pragma HLS STREAM variable=med_in depth=16

    // Output Streams for Boost, Median and Sketch
    hls::stream<uint> boost_out("boost_outputstream"),
        med_out("median_outputstream"), sketch_out("sketch_outputstream");
   #pragma HLS STREAM variable=boost_out depth=16
   #pragma HLS STREAM variable=med_out depth=16
   #pragma HLS STREAM variable=sketch_out depth=16

    int size = width * height;

    input_stage(input, boost_in, med_in, size);
    boost_stage(boost_in, boost_out, width, height);
    median_stage(med_in, med_out, width, height);
    sketch_stage(boost_out, med_out, sketch_out, size);
    output_stage(output, sketch_out, width, height);
}
}
