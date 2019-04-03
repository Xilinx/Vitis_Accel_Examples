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
    This Watermark Application example demonstrate to utilized both DDRs and 
    full bandwidth. In Watermark application, kernel has to apply a fixed
    watermark (Here is it Xilinx First Character 'X') into a given Image and 
    write the output image. 
    Inside Host code, input image is placed into Bank0 and Kernel will read
    input Image and write the output image to Bank1.
    To utilized the both Banks fully Kernel is implemented to do burst read 
    input image with full datawidth of 512 and do the burst write of output 
    image with full datawidth of 512. 
    As Kernel is accessing Sequentially from both the DDR, so kernel with get the
    Best memory access bandwidth from both DDRs and will execute with good 
    performance.
*******************************************************************************/

//Including to use ap_int<> datatype
#include <ap_int.h>

//Water Mark Defines
#define CHANNELS  3     //Three Channels (R, G and B)
#define WATERMARK_HEIGHT 16
#define WATERMARK_WIDTH 16

#define DATAWIDTH   512
#define DATA_SIZE (DATAWIDTH / 32) // Data Size is 16 (512/32 = 16)
typedef ap_uint<DATAWIDTH> TYPE;

//Per Memory Access getting 16 pixels


//function declaration
int saturatedAdd(int x, int y);

//TRIPCOUNT identifier
const unsigned int c_image_size = 2*(DATAWIDTH*WATERMARK_HEIGHT*WATERMARK_WIDTH*CHANNELS) / DATA_SIZE;

extern "C" {
void apply_watermark(TYPE *input, TYPE *output, int width, int height) {
// Using Separate interface bundle gmem0 and gmem1 for both argument
// input and output. It will allow user to create two separate interfaces
// and as a result allow kernel to access both interfaces simultaneous. 
#pragma HLS INTERFACE m_axi port=input  offset=slave bundle=gmem0
#pragma HLS INTERFACE m_axi port=output offset=slave bundle=gmem1
#pragma HLS INTERFACE s_axilite port=input  bundle=control
#pragma HLS INTERFACE s_axilite port=output bundle=control
#pragma HLS INTERFACE s_axilite port=width  bundle=control
#pragma HLS INTERFACE s_axilite port=height  bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control


    //WaterMark Image of 16x16 size
    int watermark[WATERMARK_HEIGHT][WATERMARK_WIDTH] = 
    {
      { 0, 0,        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,        0 },
      { 0, 0x0f0f0f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x0f0f0f, 0 },
      { 0, 0, 0x0f0f0f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x0f0f0f, 0, 0 }, 
      { 0, 0, 0, 0x0f0f0f, 0, 0, 0, 0, 0, 0, 0, 0, 0x0f0f0f, 0, 0, 0 },
      { 0, 0, 0, 0, 0x0f0f0f, 0, 0, 0, 0, 0, 0, 0x0f0f0f, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0, 0x0f0f0f, 0, 0, 0, 0, 0x0f0f0f, 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0, 0, 0x0f0f0f, 0, 0, 0x0f0f0f, 0, 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0, 0, 0, 0x0f0f0f, 0x0f0f0f, 0, 0, 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0, 0, 0, 0x0f0f0f, 0x0f0f0f, 0, 0, 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0, 0, 0x0f0f0f, 0, 0, 0x0f0f0f, 0, 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0, 0x0f0f0f, 0, 0, 0, 0, 0x0f0f0f, 0, 0, 0, 0, 0 },
      { 0, 0, 0, 0, 0x0f0f0f, 0, 0, 0, 0, 0, 0, 0x0f0f0f, 0, 0, 0, 0 },
      { 0, 0, 0, 0x0f0f0f, 0, 0, 0, 0, 0, 0, 0, 0, 0x0f0f0f, 0, 0, 0 },
      { 0, 0, 0x0f0f0f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x0f0f0f, 0, 0 }, 
      { 0, 0x0f0f0f, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0x0f0f0f, 0 },
      { 0, 0,        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,        0 },
    } ;
    
    int imageSize = width * height; // Total Number of Pixels
    //As memory access is 512bit wide  so total 16 pixels can be access at a time,
    // so calculating total number of Memory accesses which are needed to entire
    // Image
    int size = ( (imageSize-1) / DATA_SIZE ) + 1; 


    // Process the whole image 
    image_traverse: for (int idx = 0, x = 0 , y = 0  ; idx < size ; ++idx, x+= DATA_SIZE) {
    #pragma HLS LOOP_TRIPCOUNT min=c_image_size max=c_image_size
    #pragma HLS PIPELINE II=1
      // Read the next 16 Pixels
      TYPE tmp = input[idx];

      // Row Boundary Check for x 
      if (x >= width){
        x = x -width;
        ++y;
      }

      //Unrolling below loop to process all 16 pixels concurrently
      watermark: for ( int i = 0 ; i < DATA_SIZE ; i++) {
      #pragma HLS unroll
          int tmp_x = x + i;
          int tmp_y = y;
          // Row Boundary Check for x 
          if (tmp_x >= width)
          {
            tmp_x = tmp_x - width;
            tmp_y += 1; // Move to Next Row
          }

          //Calculating WaterMark x and y index
          int w_idx = tmp_y % WATERMARK_HEIGHT; 
          int w_idy = tmp_x % WATERMARK_WIDTH;
          tmp.range(32*(i+1) -1, 32*i)     = saturatedAdd(tmp.range(32*(i+1) -1, 32*i), watermark[w_idy][w_idx]) ;
      }

      //Write the Next 16 Pixels result to output memory
      output[idx] = tmp ;
    }
}
}

int saturatedAdd(int x, int y)
{
    // Separate into the different channels

    //Red Channel
    int redX = x & 0xff ;
    int redY = y & 0xff ;
    int redOutput ;
    
    //Green Channel
    int greenX = (x & 0xff00) >> 8 ;
    int greenY = (y & 0xff00) >> 8 ;
    int greenOutput ;
    
    //Blue Channel
    int blueX = (x & 0xff0000) >> 16 ;
    int blueY = (y & 0xff0000) >> 16 ;
    int blueOutput ;
    
    //Calculating Red 
    if (redX + redY > 255){
        redOutput = 255 ;
    }else{
      redOutput = redX + redY ;
    }
    
    //Calculating Green
    if (greenX + greenY > 255){
      greenOutput = 255 ;
    }else{
      greenOutput = greenX + greenY ;
    }
    
    //Calculating Blue
    if (blueX + blueY > 255){
      blueOutput = 255 ;
    }else {
      blueOutput = blueX + blueY ;
    }
   
    // Combining all channels into one 
    int combinedOutput = 0 ;
    combinedOutput |= redOutput ;
    combinedOutput |= (greenOutput << 8) ;
    combinedOutput |= (blueOutput << 16) ;
    return combinedOutput ;
}
