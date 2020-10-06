/**
* Copyright (C) 2020 Xilinx, Inc
*
* Licensed under the Apache License, Version 2.0 (the "License"). You may
* not use this file except in compliance with the License. A copy of the
* License is located at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
* WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
* License for the specific language governing permissions and limitations
* under the License.
*/

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
