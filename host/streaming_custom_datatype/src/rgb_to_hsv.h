/**********
Copyright (c) 2020, Xilinx, Inc.
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
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/

#pragma once

#include "ap_axi_sdata.h"
#include "ap_int.h"
#include "hls_stream.h"

constexpr unsigned int DWIDTH = 32;
using pkt = qdma_axis<DWIDTH, 0, 0, 0>;

// TRIPCOUNT identifier
constexpr unsigned int c_image_size = 16384;

// Custom Data Type for RGB Image Pixel containing Red(r), Green(g) and Blue(b)
// element. It is recommended to make custom datatype multiple of 32 bit to use
// stream
// efficiently.
struct RGBcolor {
  unsigned char r;
  unsigned char g;
  unsigned char b;

  RGBcolor() = default;

  RGBcolor(ap_int<32> d) {
    r = d.range(7, 0);
    g = d.range(15, 8);
    b = d.range(23, 16);
  }

} __attribute__((packed, aligned(4)));

// Custom Data Type for HSV Image Pixel containing Hue(h), Saturation(s),
// and Value(v) element. It is recommended to make custom datatype multiple of
// 32 bit to use stream
// efficiently.
struct HSVcolor {
  unsigned char h;
  unsigned char s;
  unsigned char v;

  ap_int<32> packed() { return ap_int<8>(h), ap_int<8>(s), ap_int<8>(v); }

} __attribute__((packed, aligned(4)));
