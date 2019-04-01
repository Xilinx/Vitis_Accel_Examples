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
    
    This is a CNN (Convolutional Neural Network) convolution layer based example
    to showcase the effectiveness of using multiple compute units when the base
    algorithm consists of multiple nested loops with large loop count. The main
    aim of this example is to help developer to overcome clock frequency issues
    and achieve better performance.

*******************************************************************************/

/*

Kernel Description (Good Example) :
   
    This example uses multiple compute units & wide range of work items 
    abstraction in C/C++ HLS flow by invoking the kernel in non-blocking fashion
    from host as many times as the total number of compute units / Work Groups
    (OpenCL) used. It also presents the uniform local memory alignment when
    multiple work_group/compute units are used. 
    
    Note : Naive version (Bad Example) version is in the file cnn_convolution_bad.cpp
    
    Arguments :
    
        int *image   (input )  --> Input Image    
        int *weight  (input )  --> Input Weights  
        int *out     (output)  --> Output filters 
        int  size    (input )  --> Output size

    Kernel Configuration :
        
        1. Output Channels    = 256
        2. Work Groups        = Number of Compute Units
        3. Work Items         = One per Work_Group (Compute Unit) 

        -----------------------------------------------------
        | Parameter     | Value |   Description             |
        -----------------------------------------------------
        | Channels      | 96    | #Input Channels           |                            
        -----------------------------------------------------
        | IHeight       | 27    | Input Image Height        |
        -----------------------------------------------------
        | IWidth        | 27    | Input Image Width         |
        -----------------------------------------------------
        | Window        | 5     | Convolution Window Size   |
        -----------------------------------------------------
        | Stride        | 1     | Convolution Stride        |
        -----------------------------------------------------
        | Padding       | 2     | Convolution Image Padding |
        -----------------------------------------------------
        | OutputFilters | 256   | Output Filters/Images     |
        -----------------------------------------------------
        | OHeight       | 27    | Output Image Height       |
        -----------------------------------------------------
        | OWidth        | 27    | Output Image Width        |
        -----------------------------------------------------


    Memory Usage (Local Buffer (Per Work_Group / Compute Unit)):
        
        1. Image    ~ (IHeight x IWidth x Channels):[2.84 x 96 KB]
        2. Weights  ~ (Channels x Window x Window):[96 x 0.09 KB]
        3. Output   ~ (OHeight x OWidth):[2.84 KB]

    Reference : 
             
        To understand Convolution Layer of a CNN better please refer to
        website below (Convolution Demo Animation in the link).
                                                     
        Link: http://cs231n.github.io/convolutional-networks/
*/

#include "defns.h"

void copy_weight(int *weight, int wgt_lcl[WInChan][WSize * WSize], int output)
{
#pragma HLS INLINE    
    // Calculate each work_item's weight matrix location 
    int stride = output * WInChan * WSize * WSize;
    
    // Each work_item copies weight matrix from DDR to local buffer
    readWt: for(int itr = 0, i = 0, j = 0; itr < WInChan * WSize * WSize; itr++,j++) {
    #pragma HLS PIPELINE II=1
        if(j == WSize * WSize) {j = 0; i++;}
        wgt_lcl[i][j] = weight[stride + itr];
    }
}

void copy_output(int *out, int out_lcl[OSize * OSize], int output)
{
#pragma HLS INLINE
    // Calculate each work_item's result update location
    int stride = output * OSize * OSize;
    
    // Work_item updates output filter/image in DDR
    writeOut: for(int itr = 0; itr < OSize * OSize; itr++) {
    #pragma HLS PIPELINE II=1
        out[stride + itr] = out_lcl[itr];
    }
}

void convolution_operation(int img_lcl[IChan][ISize * ISize], int wgt_lcl[WInChan][WSize * WSize], int out_lcl[OSize * OSize],int output, int y, int x, int i_chan)
{
#pragma HLS INLINE

    // Holds temporary accumulator values
    short acc[IChan][WSize][WSize]; 
    #pragma HLS ARRAY_PARTITION variable=acc complete dim=1

    // Holds Image Padding Boundary Check Variables
    int xVal_base = x * Stride - Padding;
    int yVal = y * Stride - Padding;

    // Runs over filter window    
    convYaxis: for(int i = 0; i < WSize; i++,yVal++){
        // Runs over filter window
        convXaxis: for(int j = 0, xVal = xVal_base ; j < WSize; j++,xVal++){
        #pragma HLS PIPELINE II=1
            // Runs over each of the input channels
            convInchan: for(int input = 0; input < IChan; input++){
                
                // Convolution operation
                if(yVal >= 0 && yVal < ISize && xVal >= 0 && xVal < ISize) {
                    acc[input][i][j] =  (short) img_lcl[input][yVal * ISize + xVal] *
                                        (short) wgt_lcl[input][i * WSize + j];
                }
                else {
                    acc[input][i][j] = 0;
                }
            }
        }
    }
    
    // Summation of temporary accumulator buffer
    short sum = 0;
    accJ: for(int j = 0; j < WSize;j++) {
        accK: for(int k = 0; k < WSize; k++) {
        #pragma HLS PIPELINE II=1
            accI: for(int i = 0; i < i_chan; i++) {
            #pragma HLS LOOP_TRIPCOUNT min=c_ichan max=c_ichan
              sum += acc[i][j][k];
        }
        }
    }

    // Update output pixel
    out_lcl[y * OSize + x] = sum;
}

extern "C"
{
    void cnn_GOOD(
            int *image,         // Read-Only Image 
            int *weights,       // Read-Only Weight Matrix
            int *out,           // Output Filters/Images
            int size,           // Size of Output Data
            int i_chan,         // Input Channels
            int o_chan,         // Output Channels
            int global_id,      
            int global_threads  
            )
    {
    #pragma HLS INTERFACE m_axi port=image offset=slave bundle=gmem
    #pragma HLS INTERFACE m_axi port=weights offset=slave bundle=gmem
    #pragma HLS INTERFACE m_axi port=out offset=slave bundle=gmem

    #pragma HLS INTERFACE s_axilite port=image bundle=control
    #pragma HLS INTERFACE s_axilite port=weights bundle=control
    #pragma HLS INTERFACE s_axilite port=out bundle=control
    #pragma HLS INTERFACE s_axilite port=size bundle=control
    #pragma HLS INTERFACE s_axilite port=i_chan bundle=control
    #pragma HLS INTERFACE s_axilite port=o_chan bundle=control
    #pragma HLS INTERFACE s_axilite port=global_id bundle=control
    #pragma HLS INTERFACE s_axilite port=global_threads bundle=control
    #pragma HLS INTERFACE s_axilite port=return bundle=control
    
        // Local Buffer to Hold Input Image
        int img_lcl[IChan][ISize * ISize];
        #pragma HLS ARRAY_PARTITION variable=img_lcl complete dim=1
        
        // Local Buffer to Hold Output Filters/Images
        int out_lcl[OSize * OSize];

        // Local Buffer to Hold Weight Matrix;
        int wgt_lcl[WInChan][WSize * WSize];
        #pragma HLS ARRAY_PARTITION variable=wgt_lcl complete dim=1
        
        // Burst Read Image
        readImg: for(int itr = 0, i = 0, j = 0; itr < i_chan * ISize * ISize; itr++, j++) {
        #pragma HLS LOOP_TRIPCOUNT min=c_ichan*c_isize*c_isize max=c_ichan*c_isize*c_isize
        #pragma HLS PIPELINE II=1
            if(j == ISize * ISize) {j = 0; i++;}
                img_lcl[i][j] = image[itr];
        }

        // Calculate each work_item's start and end point (Output Filters/Images)
        int thread_work_start = global_id * o_chan / global_threads;
        int thread_work_end   = (global_id + 1) * o_chan / global_threads;
     
        // Operate on Image Feature Maps & Weight Matrix to produce Output
        // Feature Maps. This Algorithm Operates on Large Weight Matrix Data.
        // This Example uses Creates Multiple Compute Units and makes use of
        // Thread Level Abstraction by Invoking the Kernel as much as #Compute_Units
        // Created (Work Groups)
        // 
        // Description :
        //       
        //          outthread : This Loop Operates on Output Filters/Images. Based
        //                      on the work load, Each WorkItem is responsible for
        //                      producing output Filters/Images.
        //          outYaxis  : This Loop Operates on Output Filter/Image Y Axis
        //          outXaxis  : This Loop Operates on Output Filter/Image X Axis

        outThread: for(int output = thread_work_start; output < thread_work_end; output++) {
        #pragma HLS LOOP_TRIPCOUNT min=c_ochan/4 max=c_ochan/4
            
            // Burst read weight data from global to local buffer
            copy_weight(weights, wgt_lcl, output);

            outYaxis: for(int y = 0; y < OSize; y++) {
                outXaxis: for(int x = 0; x < OSize; x++) {
                   // Perform convolution for the current 'pixel'
                   convolution_operation(img_lcl, wgt_lcl, out_lcl, output, y, x, i_chan);
                }
            }
            
            // Burst write output
            copy_output(out, out_lcl, output);
        }
        
        return;
    }
}
