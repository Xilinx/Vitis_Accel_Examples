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
#include "simplebmp.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int writebmp(char* filename, struct bmp_t* bitmap) {
    // 24 bpp uncompressed

    FILE* fp = fopen(filename, "w+b");
    if (fp == nullptr) return -1;

    // compute dib entries
    bitmap->header.dibheadersize = 40;
    bitmap->header.dibwidth = bitmap->width;
    bitmap->header.dibheight = bitmap->height;
    bitmap->header.dibplane = 1;
    bitmap->header.dibdepth = 24;
    bitmap->header.dibcompression = 0;
    bitmap->header.dibsize = (bitmap->width) * (bitmap->height) * (bitmap->header.dibdepth / 8);
    bitmap->header.dibhor = 2835;
    bitmap->header.dibver = 2835;
    bitmap->header.dibpal = 0;
    bitmap->header.dibimportant = 0;

    // compute header entries
    bitmap->header.headerB = 'B';
    bitmap->header.headerM = 'M';
    bitmap->header.headerpixelsoffset = 54;
    bitmap->header.headerbmpsize = bitmap->header.dibsize + bitmap->header.headerpixelsoffset;
    bitmap->header.headerapp0 = 0;
    bitmap->header.headerapp1 = 0;

    // write header
    fwrite(&(bitmap->header.headerB), 1, 1, fp);
    fwrite(&(bitmap->header.headerM), 1, 1, fp);
    fwrite(&(bitmap->header.headerbmpsize), 4, 1, fp);
    fwrite(&(bitmap->header.headerapp0), 2, 1, fp);
    fwrite(&(bitmap->header.headerapp1), 2, 1, fp);
    fwrite(&(bitmap->header.headerpixelsoffset), 4, 1, fp);

    // write dib header
    fwrite(&(bitmap->header.dibheadersize), 4, 1, fp);
    fwrite(&(bitmap->header.dibwidth), 4, 1, fp);
    fwrite(&(bitmap->header.dibheight), 4, 1, fp);
    fwrite(&(bitmap->header.dibplane), 2, 1, fp);
    fwrite(&(bitmap->header.dibdepth), 2, 1, fp);
    fwrite(&(bitmap->header.dibcompression), 4, 1, fp);
    fwrite(&(bitmap->header.dibsize), 4, 1, fp);
    fwrite(&(bitmap->header.dibhor), 4, 1, fp);
    fwrite(&(bitmap->header.dibver), 4, 1, fp);
    fwrite(&(bitmap->header.dibpal), 4, 1, fp);
    fwrite(&(bitmap->header.dibimportant), 4, 1, fp);

    // write pixels
    fwrite(bitmap->pixels, bitmap->header.dibsize, 1, fp);

    if (ferror(fp)) return -1;

    fclose(fp);
    return 0;
}

int readbmp(char* filename, struct bmp_t* bitmap) {
    //-1 file access error
    //-2 invalid BMP
    //-3 memory allocation error
    FILE* fp = fopen(filename, "r+b");
    if (fp == nullptr) return -1;

    // read header
    fread(&(bitmap->header.headerB), 1, 1, fp);
    fread(&(bitmap->header.headerM), 1, 1, fp);
    fread(&(bitmap->header.headerbmpsize), 4, 1, fp);
    fread(&(bitmap->header.headerapp0), 2, 1, fp);
    fread(&(bitmap->header.headerapp1), 2, 1, fp);
    fread(&(bitmap->header.headerpixelsoffset), 4, 1, fp);
    // read dib header
    fread(&(bitmap->header.dibheadersize), 4, 1, fp);
    fread(&(bitmap->header.dibwidth), 4, 1, fp);
    fread(&(bitmap->header.dibheight), 4, 1, fp);
    fread(&(bitmap->header.dibplane), 2, 1, fp);
    fread(&(bitmap->header.dibdepth), 2, 1, fp);
    fread(&(bitmap->header.dibcompression), 4, 1, fp);
    fread(&(bitmap->header.dibsize), 4, 1, fp);
    fread(&(bitmap->header.dibhor), 4, 1, fp);
    fread(&(bitmap->header.dibver), 4, 1, fp);
    fread(&(bitmap->header.dibpal), 4, 1, fp);
    fread(&(bitmap->header.dibimportant), 4, 1, fp);

    if (ferror(fp)) return -1;

    // validate header
    // header
    if (bitmap->header.headerB != 'B') return -2;
    if (bitmap->header.headerM != 'M') return -2;
    // headerbmpsize
    if (bitmap->header.headerapp0 != 0) return -2;
    if (bitmap->header.headerapp1 != 0) return -2;
    if (bitmap->header.headerpixelsoffset != 54) return -2;
    // dib header
    if (bitmap->header.dibheadersize != 40) return -2;
    bitmap->width = bitmap->header.dibwidth;
    bitmap->height = bitmap->header.dibheight;
    if (bitmap->header.dibplane != 1) return -2;
    if (bitmap->header.dibdepth != 24) return -2;
    if (bitmap->header.dibcompression != 0) return -2;
    if (bitmap->header.dibsize != (bitmap->header.dibwidth * bitmap->header.dibheight * 3)) return -2;
    // dibsize do nothing yet
    // dibhor unused
    // dibver unused
    if (bitmap->header.dibpal != 0) return -2;
    if (bitmap->header.dibimportant != 0) return -2;

    // read pixels
    bitmap->pixels = (uint32_t*)malloc(bitmap->header.dibsize);
    if (bitmap->pixels == nullptr) return -3;
    fread(bitmap->pixels, bitmap->header.dibsize, 1, fp);

    if (ferror(fp)) return -1;

    fclose(fp);
    return 0;
}
