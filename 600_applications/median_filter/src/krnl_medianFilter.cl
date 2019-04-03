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

#define SIZE       9
#define CHANNELS   3
#define MAX_WIDTH  1024

#define imin(X, Y) (((X) < (Y)) ? (X) : (Y))
#define imax(X, Y) (((X) > (Y)) ? (X) : (Y))

//
// Get value in vector
//
uint getValue(uint16 vec, int index) {
  if (index == 0)  return vec.s0;
  if (index == 1)  return vec.s1;
  if (index == 2)  return vec.s2;
  if (index == 3)  return vec.s3;
  if (index == 4)  return vec.s4;
  if (index == 5)  return vec.s5;
  if (index == 6)  return vec.s6;
  if (index == 7)  return vec.s7;
  if (index == 8)  return vec.s8;
  if (index == 9)  return vec.s9;
  if (index == 10) return vec.sa;
  if (index == 11) return vec.sb;
  if (index == 12) return vec.sc;
  if (index == 13) return vec.sd;
  if (index == 14) return vec.se;
  if (index == 15) return vec.sf;
  return 0;
}

//
// Get median value
// 
uint getMedian(int channel, uint* rgb) {
  uint mask = 0xFF << (8*channel);
  
  // Extract next color channel
  uint c[SIZE];
  for (int p = 0; p < SIZE; p++) {
    c[p] = rgb[p] & mask;
  }

  // Perform sort to find median of current channel
  uint minval = imin(c[0], c[1]);
  uint maxval = imax(c[0], c[1]);
  c[0] = minval;
  c[1] = maxval;

  minval = imin(c[3], c[2]);
  maxval = imax(c[3], c[2]);
  c[3] = minval;
  c[2] = maxval;

  minval = imin(c[2], c[0]);
  maxval = imax(c[2], c[0]);
  c[2] = minval;
  c[0] = maxval;

  minval = imin(c[3], c[1]);
  maxval = imax(c[3], c[1]);
  c[3] = minval;
  c[1] = maxval;

  minval = imin(c[1], c[0]);
  maxval = imax(c[1], c[0]);
  c[1] = minval;
  c[0] = maxval;

  minval = imin(c[3], c[2]);
  maxval = imax(c[3], c[2]);
  c[3] = minval;
  c[2] = maxval;

  minval = imin(c[5], c[4]);
  maxval = imax(c[5], c[4]);
  c[5] = minval;
  c[4] = maxval;

  minval = imin(c[7], c[8]);
  maxval = imax(c[7], c[8]);
  c[7] = minval;
  c[8] = maxval;

  minval = imin(c[6], c[8]);
  maxval = imax(c[6], c[8]);
  c[6] = minval;
  c[8] = maxval;

  minval = imin(c[6], c[7]);
  maxval = imax(c[6], c[7]);
  c[6] = minval;
  c[7] = maxval;

  minval = imin(c[4], c[8]);
  maxval = imax(c[4], c[8]);
  c[4] = minval;
  c[8] = maxval;

  minval = imin(c[4], c[6]);
  maxval = imax(c[4], c[6]);
  c[4] = minval;
  c[6] = maxval;

  minval = imin(c[5], c[7]);
  maxval = imax(c[5], c[7]);
  c[5] = minval;
  c[7] = maxval;

  minval = imin(c[4], c[5]);
  maxval = imax(c[4], c[5]);
  c[4] = minval;
  c[5] = maxval;

  minval = imin(c[6], c[7]);
  maxval = imax(c[6], c[7]);
  c[6] = minval;
  c[7] = maxval;

  minval = imin(c[0], c[8]);
  maxval = imax(c[0], c[8]);
  c[0] = minval;
  c[8] = maxval;

  c[4] = imax(c[0], c[4]);
  c[5] = imax(c[1], c[5]);

  c[6] = imax(c[2], c[6]);
  c[7] = imax(c[3], c[7]);

  c[4] = imin(c[4], c[6]);
  c[5] = imin(c[5], c[7]);

  return imin(c[4], c[5]);
}

//
// Median filter
// 
__kernel __attribute__ ((reqd_work_group_size(1, 1, 1)))
void median(__global const uint16* input, __global uint16* output, int width, int height) {
  local uint16 linebuf0[MAX_WIDTH/16];
  local uint16 linebuf1[MAX_WIDTH/16];
  local uint16 linebuf2[MAX_WIDTH/16];
  int width16 = width >> 4;
  uint16 result16;
  
  for (int line = 0; line < height; line++) {
    int yindex = line*width16;
    
    // Fetch Lines
    if (line == 0) {
      async_work_group_copy(linebuf0, input, width16, 0);
      async_work_group_copy(linebuf1, input, width16, 0);
      async_work_group_copy(linebuf2, input + width16, width16, 0);
    }
    else if (line < height-1) {
      if (line % 3 == 0)
        async_work_group_copy(linebuf0, input + (line+1)*width16, width16, 0);
      else if (line % 3 == 1)
        async_work_group_copy(linebuf1, input + (line+1)*width16, width16, 0);
      else if (line % 3 == 2)
        async_work_group_copy(linebuf2, input + (line+1)*width16, width16, 0);
    }
    barrier(CLK_LOCAL_MEM_FENCE);
  
    __attribute__((xcl_pipeline_loop))
    for (int x=0; x < width16; x++) {
      __attribute__((opencl_unroll_hint))
      for (int i=0; i < 16; i++) {
        // Get pixels within 3x3 aperture
        uint rgb[SIZE];
        rgb[0] = (i == 0) ? getValue(linebuf0[x], i) : getValue(linebuf0[x], i-1);
        rgb[1] = getValue(linebuf0[x], i);
        rgb[2] = (i == 15) ? getValue(linebuf0[x+1], 0) : getValue(linebuf0[x], i+1);
        
        rgb[3] = (i == 0) ? getValue(linebuf1[x], i) : getValue(linebuf1[x], i-1);
        rgb[4] = getValue(linebuf1[x], i);
        rgb[5] = (i == 15) ? getValue(linebuf1[x+1], 0) : getValue(linebuf1[x], i+1);
        
        rgb[6] = (i == 0) ? getValue(linebuf2[x], i) : getValue(linebuf2[x], i-1);
        rgb[7] = getValue(linebuf2[x], i);
        rgb[8] = (i == 15) ? getValue(linebuf2[x+1], 0) : getValue(linebuf2[x], i+1);
        
        uint result = 0;
  
        // Iterate over all color channels
        __attribute__((opencl_unroll_hint))
        for (int channel = 0; channel < CHANNELS; channel++) {
          result |= getMedian(channel, rgb);
        }
  
        // Store result into memory
        if (i == 0)  result16.s0 = result;
        if (i == 1)  result16.s1 = result;
        if (i == 2)  result16.s2 = result;
        if (i == 3)  result16.s3 = result;
        if (i == 4)  result16.s4 = result;
        if (i == 5)  result16.s5 = result;
        if (i == 6)  result16.s6 = result;
        if (i == 7)  result16.s7 = result;
        if (i == 8)  result16.s8 = result;
        if (i == 9)  result16.s9 = result;
        if (i == 10) result16.sa = result;
        if (i == 11) result16.sb = result;
        if (i == 12) result16.sc = result;
        if (i == 13) result16.sd = result;
        if (i == 14) result16.se = result;
        if (i == 15) result16.sf = result;
      }
      
      output[yindex + x] = result16;
    }
  }
}
