/**
* Copyright (C) 2019-2021 Xilinx, Inc
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

// Min and Max Macro Function
#define imin(X, Y) (((X) < (Y)) ? (X) : (Y))
#define imax(X, Y) (((X) > (Y)) ? (X) : (Y))

// TRIPCOUNT identifier
const unsigned int c_image_size = 16384;

// Custom Data Type for RGB Image Pixel containing Red(r), Green(g) and Blue(b)
// element. It is recommended to make custom datatype multiple of 32 bit to use
// global
// memory access efficiently.
typedef struct RGBcolor_struct {
    unsigned char r;
    unsigned char g;
    unsigned char b;
} __attribute__((packed, aligned(4))) RGBcolor;

// Custom Data Type for HSV Image Pixel containing Hue(h), Saturation(s),
// and Value(v) element. It is recommended to make custom datatype multiple of
// 32 bit to use global
// memory access efficiently.
typedef struct HSVcolor_struct {
    unsigned char h;
    unsigned char s;
    unsigned char v;
} __attribute__((packed, aligned(4))) HSVcolor;
