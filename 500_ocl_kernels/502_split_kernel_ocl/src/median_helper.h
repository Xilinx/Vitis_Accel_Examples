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

#ifndef __MEDIAN_HELPER_H__
#define __MEDIAN_HELPER_H__

// This file contains helper functions for performing median filtering

// Get median of the current RGB window for the given channel
#ifndef SW_FLOW
uint __attribute__((always_inline)) getChannelMedian(int channel, uint *rgbWindow)
#else
uint getChannelMedian(int channel, uint *rgbWindow)
#endif
{
  uint mask = 0xFF << (8*channel);
  
  // Extract next color channel
  uint c[SIZE];
  for (int p = 0; p < SIZE; p++) {
    c[p] = rgbWindow[p] & mask;
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

// Get median value over the current RGB window
#ifndef SW_FLOW
uint __attribute__((always_inline)) getMedian(uint *rgbWindow)
#else
uint getMedian(uint *rgbWindow)
#endif
{
    uint result = 0;
    // Iterate over all color channels
    for (int channel = 0; channel < CHANNELS; channel++){
        result |= getChannelMedian(channel, rgbWindow);
    }
    return result;
}

// Get pixels within 3x3 aperture based of the current lines
// Reuse old values to achieve pipeline II = 1
#ifndef SW_FLOW
void __attribute__((always_inline)) getMedianWindow(uint *rgbWindow, uint linebuf[MEDIAN_WINDOW][MAX_WIDTH], unsigned char rowAddr[MEDIAN_WINDOW+1], int x, int width)
#else
void getMedianWindow(uint *rgbWindow, uint linebuf[MEDIAN_WINDOW][MAX_WIDTH], unsigned char rowAddr[MEDIAN_WINDOW+1], int x, int width)
#endif
{
    if(x == 0)
    {
        rgbWindow[0] = linebuf[rowAddr[0]][x];
        rgbWindow[1] = linebuf[rowAddr[0]][x];
        rgbWindow[2] = linebuf[rowAddr[0]][x + 1];

        rgbWindow[3] = linebuf[rowAddr[1]][x];
        rgbWindow[4] = linebuf[rowAddr[1]][x];
        rgbWindow[5] = linebuf[rowAddr[1]][x + 1];

        rgbWindow[6] = linebuf[rowAddr[2]][x];
        rgbWindow[7] = linebuf[rowAddr[2]][x];
        rgbWindow[8] = linebuf[rowAddr[2]][x + 1];
    }
    else if(x == 1)
    {
        rgbWindow[0] = rgbWindow[1];
        rgbWindow[1] = linebuf[rowAddr[0]][x];
        rgbWindow[2] = linebuf[rowAddr[0]][x + 1];

        rgbWindow[3] = rgbWindow[4];
        rgbWindow[4] = linebuf[rowAddr[1]][x];
        rgbWindow[5] = linebuf[rowAddr[1]][x + 1];

        rgbWindow[6] = rgbWindow[7];
        rgbWindow[7] = linebuf[rowAddr[2]][x];
        rgbWindow[8] = linebuf[rowAddr[2]][x + 1];
    }
    else
    {
        rgbWindow[0] = rgbWindow[1];
        rgbWindow[1] = rgbWindow[2];
        rgbWindow[2] = (x == width - 1) ? linebuf[rowAddr[0]][x] : linebuf[rowAddr[0]][x + 1];

        rgbWindow[3] = rgbWindow[4];
        rgbWindow[4] = rgbWindow[5];
        rgbWindow[5] = (x == width - 1) ? linebuf[rowAddr[1]][x] : linebuf[rowAddr[1]][x + 1];

        rgbWindow[6] = rgbWindow[7];
        rgbWindow[7] = rgbWindow[8];
        rgbWindow[8] = (x == width - 1) ? linebuf[rowAddr[2]][x] : linebuf[rowAddr[2]][x + 1];
    }
}

#endif
