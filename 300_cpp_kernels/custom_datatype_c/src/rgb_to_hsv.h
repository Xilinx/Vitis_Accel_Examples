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

//Min and Max Macro Function
#define imin(X, Y) (((X) < (Y)) ? (X) : (Y))
#define imax(X, Y) (((X) > (Y)) ? (X) : (Y))

//TRIPCOUNT identifier
const unsigned int c_image_size = 16384;

//Custom Data Type for RGB Image Pixel containing Red(r), Green(g) and Blue(b) 
//element. Extra Pad element is added to make struct size to 32bit.
// It is recommended to make custom datatype multiple of 32 bit to use global 
// memory access efficiently.
typedef struct RGBcolor_struct
{
  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char pad;
} RGBcolor;


//Custom Data Type for HSV Image Pixel containing Hue(h), Saturation(s), 
//and Value(v) element. Extra pad element is added to make struct size to 32bit.
// It is recommended to make custom datatype multiple of 32 bit to use global 
// memory access efficiently.
typedef struct HSVcolor_struct
{
  unsigned char h;
  unsigned char s;
  unsigned char v;
  unsigned char pad;
}HSVcolor;
