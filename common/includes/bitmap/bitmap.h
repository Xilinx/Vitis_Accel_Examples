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
#ifndef BITMAP_DOT_H
#define BITMAP_DOT_H

#include <stdlib.h>

class BitmapInterface {
   private:
    char* core;
    char* dib;
    const char* filename;
    int* image;

    // Core header information
    unsigned short magicNumber;
    unsigned int fileSize;
    unsigned int offsetOfImage;

    // DIB information
    int sizeOfDIB;
    int sizeOfImage;
    int height;
    int width;

   public:
    BitmapInterface(const char* f);
    ~BitmapInterface();

    bool readBitmapFile();
    bool writeBitmapFile(int* otherImage = nullptr);

    inline int* bitmap() { return image; }
    unsigned int numPixels() { return sizeOfImage / 3; }

    inline int getHeight() { return height; }
    inline int getWidth() { return width; }
};

#endif
