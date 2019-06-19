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
   
    This example uses multiple compute units & wide range of work items which
    process Output filters when convolution operation is triggered. It also
    presents the uniform local memory alignment when multiple work_group/compute
    units are used. 
    
    Note : Naive version (Bad Example) version is in the file cnn_convolution_bad.cl
    
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

// Tripcount identifiers
__constant int c_wsize = WSize;
__constant int c_ichan = IChan;
__constant int c_osize = OSize;
__constant int c_isize = ISize;
__constant int c_ochan = OChan;

void __attribute__((always_inline)) copy_weight(__global int *weight, int wgt_lcl[WInChan][WSize * WSize], int output)
{
    // Calculate each work_item's weight matrix location 
    int stride = output * WInChan * WSize * WSize;
    
    // Each work_item copies its weight data from DDR to local buffers
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_ichan*c_wsize*c_wsize, c_ichan*c_wsize*c_wsize)))
    readWt: 
    // To avoid automatic loop unrolling
    #pragma nounroll
    for(int itr = 0, i = 0, j = 0; itr < WInChan * WSize * WSize; itr++,j++) {
        if(j == WSize * WSize) {j = 0; i++;}
        wgt_lcl[i][j] = weight[stride + itr];
    }
}

void __attribute__((always_inline)) copy_output(__global int *out, int out_lcl[OSize * OSize], int output)
{
    // Calculate each work_item's result update location
    int stride = output * OSize * OSize;
    
    // Work_item updates output filter/image in DDR
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_osize*c_osize, c_osize*c_osize)))
    writeOut: for(int itr = 0; itr < OSize * OSize; itr++) {
        out[stride + itr] = out_lcl[itr];
    }
}

void __attribute__((always_inline))
    convolution_operation(int img_lcl[IChan][ISize * ISize], int wgt_lcl[WInChan][WSize * WSize], int out_lcl[OSize * OSize],int output, int y, int x, int i_chan)
{
    // Holds temporary accumulator values
    short acc[IChan][WSize][WSize] 
    __attribute__((xcl_array_partition(complete,1)));

    // Holds Image Padding Boundary Check Variables
    int xVal_base = x * Stride - Padding;
    int yVal = y * Stride - Padding;

    // Runs over filter window  
    __attribute__((xcl_loop_tripcount(c_wsize, c_wsize)))
    convYaxis: for(int i = 0; i < WSize; i++,yVal++){
        __attribute__((xcl_pipeline_loop(1)))
        // Runs over filter window
        __attribute__((xcl_loop_tripcount(c_wsize, c_wsize)))
        convXaxis: for(int j = 0, xVal = xVal_base ; j < WSize; j++, xVal++){
            // Runs over each of the input channels
            __attribute__((xcl_loop_tripcount(c_ichan, c_ichan)))
            convInchan: for(int input = 0; input < IChan; input++) {
                
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
    __attribute__((xcl_loop_tripcount(c_wsize, c_wsize)))
    accJ: for(int j = 0; j < WSize;j++)
        __attribute__((xcl_pipeline_loop(1)))
        __attribute__((xcl_loop_tripcount(c_wsize, c_wsize)))
        accK: for(int k = 0; k < WSize; k++)
            __attribute__((xcl_loop_tripcount(c_ichan, c_ichan)))
            accI: for(int i = 0; i < i_chan; i++)
                sum += acc[i][j][k];

    // Update output pixel
    out_lcl[y * OSize + x] = sum;
}

__kernel __attribute__ ((reqd_work_group_size(1, 1, 1)))
void cnn_GOOD(
        const __global int *image,      // Read-Only Image 
        const __global int *weights,    // Read-Only Weight Matrix
        __global int *out,              // Output Filters/Images
        int size,                       // Size of Output Data
        int i_chan,                     // Input Channels
        int o_chan                      // Output Channels
    )
{
    int global_id      = get_global_id(0);         // Picks Work Item global ID
    int global_threads = get_global_size(0);       // Picks Total Threads
    
    // Local Buffer to Hold Input Image
    int img_lcl[IChan][ISize * ISize] __attribute__((xcl_array_partition(complete,1)));

    // Local Buffer to Hold Output Filters/Images
    int out_lcl[OSize * OSize];

    // Local Buffer to Hold Weight Matrix;
    int wgt_lcl[WInChan][WSize * WSize] __attribute__((xcl_array_partition(complete,1)));
    
    // Copy image data from DDR to local buffer per work group (Burst Mode)
    __attribute__((xcl_pipeline_loop(1)))
    __attribute__((xcl_loop_tripcount(c_ichan*c_isize*c_isize, c_ichan*c_isize*c_isize)))
    imgcopy:for(int itr = 0, i = 0, j = 0; itr < i_chan * ISize * ISize; itr++, j++){
        if(j == ISize * ISize) {j = 0; i++;}
            img_lcl[i][j] = image[itr];
    }

    // Calculate each work_item's start and end point (Output Filters/Images)
    int thread_work_start = global_id * o_chan / global_threads;
    int thread_work_end   = (global_id + 1) * o_chan / global_threads;
    
    // Operate on Image Feature Maps & Weight Matrix to produce Output Feature
    // Maps. This Algorithm Operates on Large Weight Matrix Data. This example
    // uses OpenCL Work_Group/Work_Items concept to create multiple instances
    // of kernel Compute Units.
    // 
    // Description :
    //       
    //          outthread : This Loop Operates on Output Filters/Images. Based
    //                      on the work load, Each WorkItem is responsible for
    //                      producing output Filters/Images.
    //          outYaxis  : This Loop Operates on Output Filter/Image Y Axis
    //          outXaxis  : This Loop Operates on Output Filter/Image X Axis
    
    outthread:for(int output = thread_work_start; output < thread_work_end; output++) {
            
        // Burst read weight data from global to local buffer
        copy_weight(weights, wgt_lcl, output);
        
        __attribute__((xcl_loop_tripcount(c_osize, c_osize)))
        outYaxis:for(int y = 0; y < OSize; y++) {
            __attribute__((xcl_loop_tripcount(c_osize, c_osize)))
            outXaxis:for(int x = 0; x < OSize; x++) {
               // Perform convolution for the current 'pixel'
               convolution_operation(img_lcl, wgt_lcl, out_lcl, output, y, x, i_chan);
            }
        }
        
        // Burst write output
        copy_output(out, out_lcl, output);
    }

    return;
}
