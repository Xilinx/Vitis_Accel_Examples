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

#pragma once

// This file contains helper functions for performing boost filtering

// Get boost value over the current RGB window
uint getBoost(uint *rgbWindow)
{
#ifndef SW_FLOW
    #pragma HLS INLINE
#endif
    uint result = 0;
    
    // Iterate over all color channels
    for (int channel = 0; channel < CHANNELS; channel++){
    #ifndef SW_FLOW
        #pragma HLS UNROLL
    #endif
        uint maxVal = 0;
        uint mask = 0xFF << (8*channel);
        
        for(int i = 0; i < SIZE; i++){
            uint currVal = rgbWindow[i] & mask;
            maxVal = imax(maxVal, currVal);
        }
        
        result |= maxVal;
    }
    
    return result;
}

// Get pixels within 3x3 aperture based of the current lines
// Reuse old values to achieve pipeline II = 1
void getBoostWindow(uint *rgbWindow, uint linebuf[BOOST_WINDOW][MAX_WIDTH], unsigned char rowAddr[BOOST_WINDOW+1], int x, int width)
{
#ifndef SW_FLOW
    #pragma HLS INLINE
#endif
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

