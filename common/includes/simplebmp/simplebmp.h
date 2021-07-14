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
// Read and write uncompressed 24 bit BMP image format image
// based on http://en.wikipedia.org/wiki/BMP_file_formt
// Copyright Xilinx

#ifndef __SIMPLE_BMP
#define __SIMPLE_BMP

struct bmpheader_t {
    // Header
    char headerB;
    char headerM;
    uint32_t headerbmpsize;
    uint16_t headerapp0;
    uint16_t headerapp1;
    uint32_t headerpixelsoffset;

    // DIB header
    uint32_t dibheadersize;
    uint32_t dibwidth;
    uint32_t dibheight;
    uint16_t dibplane;
    uint16_t dibdepth;
    uint32_t dibcompression;
    uint32_t dibsize;
    uint32_t dibhor;
    uint32_t dibver;
    uint32_t dibpal;
    uint32_t dibimportant;
};

struct bmp_t {
    struct bmpheader_t header;
    uint32_t width;
    uint32_t height;
    uint32_t* pixels;
};

int writebmp(char* filename, struct bmp_t* bitmap);

int readbmp(char* filename, struct bmp_t* bitmap);
//-1 file access error
//-2 invalid BMP
//-3 memory allocation error

#endif
