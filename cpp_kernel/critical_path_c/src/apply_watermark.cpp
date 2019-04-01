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
    This example demonstrate to a coding style which could lead to critical
    path issue and design may not meet timing and failed to compile. It will 
    also restructured the code to meet design timing. 
*******************************************************************************/

//Water Mark Defines
#define CHANNELS  3     //Three Channels (R, G and B)
#define WATERMARK_HEIGHT 16
#define WATERMARK_WIDTH 16

//Per Memory Access getting 16 pixels
#define DATA_SIZE 16
typedef unsigned int uint;

//Using structure to get the full memory datawidth 512
typedef struct TYPE {
    uint data[DATA_SIZE];
} TYPE;

// Tripcount identifiers
const int c_size = DATA_SIZE;

//function declaration
int saturatedAdd(int x, int y);

extern "C" {
void apply_watermark(const TYPE* input, TYPE* output, int width, int height) {
#pragma HLS INTERFACE m_axi port=input  offset=slave bundle=gmem
#pragma HLS INTERFACE m_axi port=output  offset=slave bundle=gmem
#pragma HLS INTERFACE s_axilite port=input  bundle=control
#pragma HLS INTERFACE s_axilite port=output  bundle=control
#pragma HLS INTERFACE s_axilite port=width bundle=control
#pragma HLS INTERFACE s_axilite port=height bundle=control
#pragma HLS INTERFACE s_axilite port=return bundle=control

#pragma HLS DATA_PACK variable=input
#pragma HLS DATA_PACK variable=output

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
    
    uint imageSize = width * height; // Total Number of Pixels
    //As memory access is int16 type so total 16 pixels can be access at a time,
    // so calculating total number of Memory accesses which are needed to entire
    // Image
    uint size = ( (imageSize-1) / DATA_SIZE ) + 1; 

    // Process the whole image 
    image_traverse: for (uint idx = 0, x = 0 , y = 0  ; idx < size ; ++idx) {
    #pragma HLS PIPELINE II=1
      // Read the next 16 Pixels
      TYPE tmp = input[idx];

      // Row Boundary Check for x 
      if (x >= width){
        x = x -width;
        ++y;
      }
#if BAD_DESIGN==1
      //Unrolling below loop to process all 16 pixels concurrently but 
      // "x" variable value is referred and getting updated for each iteration
      // of loop which will lead to longer critical path and design may not meet
      // timing.
      watermark: for ( int i = 0 ; i < DATA_SIZE ; i++, x++) {
      #pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
      #pragma HLS UNROLL 
          // Row Boundary Check for x 
          if (x > width) {
            x = x -width;
            y +=1;
          }

          uint w_idy = y % WATERMARK_HEIGHT; 
          uint w_idx = x % WATERMARK_WIDTH;
          tmp.data[i]     = saturatedAdd(tmp.data[i], watermark[w_idy][w_idx]) ;
      }
#else
      //Restructured the code such a way that "x" variable is just referred
      // in each iteration. No changes in "x" variable to reduce the critical path 
      // to improve timing.
      watermark: for ( int i = 0 ; i < DATA_SIZE ; i++) {
      #pragma HLS LOOP_TRIPCOUNT min=c_size max=c_size
      #pragma HLS UNROLL
          uint tmp_x = x+i;
          uint tmp_y = y;
          // Row Boundary Check for x 
          if (tmp_x > width) {
            tmp_x = tmp_x -width;
            tmp_y +=1;
          }

          uint w_idy = tmp_y % WATERMARK_HEIGHT; 
          uint w_idx = tmp_x % WATERMARK_WIDTH;
          tmp.data[i]     = saturatedAdd(tmp.data[i], watermark[w_idy][w_idx]) ;
      }
      // "x" variable is getting updated outside the loop
      x += DATA_SIZE;
#endif

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
