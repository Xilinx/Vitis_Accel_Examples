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
#include "xcl2.hpp"
#include <vector>
#define USE_IN_HOST
#include "bitmap.h"
#include "rgb_to_hsv.h"

//Utility Function Declaration
void sw_RgbToHsv (int *in, int *out, size_t image_size);
void sw_HsvToRgb (int *in, int *out, size_t image_size);
int compareImages(int *in, int *out, size_t image_size);


int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << " <input bitmap>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];
    std::string bitmapFilename = argv[2];
    
    cl_int err;
    unsigned fileBufSize;
    
    //Read the bit map file into memory
    BitmapInterface image(bitmapFilename.data());
    bool result = image.readBitmapFile() ;
    if (!result)
    {
        std::cout << "ERROR:Unable to Read Bitmap File "<< bitmapFilename.data() << std::endl;
      return EXIT_FAILURE ;
    }
   
    //Allocate Memory in Host Memory 
    int image_size = image.numPixels();
    size_t image_size_bytes = sizeof(int) * image_size;
    std::vector<int,aligned_allocator<int>> hwRgbImage(image_size) ;
    std::vector<int,aligned_allocator<int>> hwHsvImage(image_size) ;
    std::vector<int,aligned_allocator<int>> swHsvImage(image_size) ;
    std::vector<int,aligned_allocator<int>> outRgbImage(image_size) ;

    //Copying image host buffer
    memcpy(hwRgbImage.data(),image.bitmap(),image_size_bytes);

//OPENCL HOST CODE AREA START
    std::vector<cl::Device> devices = xcl::get_xil_devices();
    cl::Device device = devices[0];

    OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(err, cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
    OCL_CHECK(err, std::string device_name = device.getInfo<CL_DEVICE_NAME>(&err));

    char* fileBuf = xcl::read_binary_file(binaryFile, fileBufSize);
    cl::Program::Binaries bins{{fileBuf, fileBufSize}};
    devices.resize(1);
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
    OCL_CHECK(err, cl::Kernel krnl_rgb2hsv(program,"rgb_to_hsv", &err));
    
    //Allocate Buffer in Global Memory
    OCL_CHECK(err, cl::Buffer buffer_rgbImage(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
            image_size_bytes,hwRgbImage.data(), &err));

    OCL_CHECK(err, cl::Buffer buffer_hsvImage(context, CL_MEM_WRITE_ONLY| CL_MEM_USE_HOST_PTR,
            image_size_bytes,hwHsvImage.data(), &err));

    OCL_CHECK(err, err = krnl_rgb2hsv.setArg(0, buffer_rgbImage));
    OCL_CHECK(err, err = krnl_rgb2hsv.setArg(1, buffer_hsvImage));
    OCL_CHECK(err, err = krnl_rgb2hsv.setArg(2, image_size));

    //Copy input RGB Image to device global memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_rgbImage},0/* 0 means from host*/));

    //Launch the Kernel
    OCL_CHECK(err, err = q.enqueueTask(krnl_rgb2hsv));

    //Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_hsvImage},CL_MIGRATE_MEM_OBJECT_HOST));
    OCL_CHECK(err, err = q.finish());

//OPENCL HOST CODE AREA END

    //Calculating sw based HSV Image 
    sw_RgbToHsv(image.bitmap(), swHsvImage.data(), image.numPixels());

    //Compare the results of the Device to the Sw Based
    int match= compareImages(swHsvImage.data(), hwHsvImage.data(),image.numPixels());

    //Converting Generated HSV to back to RGB and Writing RGB file to disk
    sw_HsvToRgb(hwHsvImage.data(), outRgbImage.data(), image.numPixels());
    image.writeBitmapFile(outRgbImage.data()) ;

    delete[] fileBuf;

    std::cout << "TEST " << (match ? "FAILED" : "PASSED") << std::endl; 
    return (match ? EXIT_FAILURE :  EXIT_SUCCESS);
}

//Utility Functions Definitions Start Here

//Convert RGB to HSV Format
void sw_RgbToHsv(int *in, int *out, size_t image_size)
{
    RGBcolor rgb;
    HSVcolor hsv;
    for(size_t i = 0 ; i < image_size ; 
            out[i] = hsv.h | (hsv.s << 8) | (hsv.v << 16), i++){
        rgb.r =  (in[i]) & 0xff;
        rgb.g = ( (in[i]) & 0xff00 ) >> 8 ;
        rgb.b = ( (in[i]) & 0xff0000 ) >> 16 ;
        unsigned char rgbMin, rgbMax;
        
        rgbMin = imin(rgb.r, (imin(rgb.g,rgb.b)));
        rgbMax = imax(rgb.r, (imax(rgb.g,rgb.b)));
        
        hsv.v = rgbMax;
        if (hsv.v == 0)
        {
            hsv.h = 0;
            hsv.s = 0;
            continue;
        }
        
        hsv.s = 255 * ((long)(rgbMax - rgbMin)) / hsv.v;
        if (hsv.s == 0)
        {
            hsv.h = 0;
            continue;
        }
        
        if (rgbMax == rgb.r)
            hsv.h = 0 + 43 * (rgb.g - rgb.b) / (rgbMax - rgbMin);
        else if (rgbMax == rgb.g)
            hsv.h = 85 + 43 * (rgb.b - rgb.r) / (rgbMax - rgbMin);
        else
            hsv.h = 171 + 43 * (rgb.r - rgb.g) / (rgbMax - rgbMin);
        }
}

//Convert RGB to HSV Format
void sw_HsvToRgb(int *in, int *out, size_t image_size)
{
    RGBcolor rgb;
    HSVcolor hsv;
    for (size_t i = 0 ; i < image_size ; 
            out[i] = rgb.r | (rgb.g << 8) | (rgb.b << 16), i++)
    {
        hsv.h = in[i] & 0xff;
        hsv.s = (in[i] & 0xff00) >> 8;
        hsv.v = (in[i] & 0xff0000) >> 16;
        unsigned char region, p, q, t;
        unsigned int h, s, v, remainder;

        if (hsv.s == 0)
        {
            rgb.r = hsv.v;
            rgb.g = hsv.v;
            rgb.b = hsv.v;
            continue;
        }

        // converting to 16 bit to prevent overflow
        h = hsv.h;
        s = hsv.s;
        v = hsv.v;

        region = h / 43;
        remainder = (h - (region * 43)) * 6; 

        p = (v * (255 - s)) >> 8;
        q = (v * (255 - ((s * remainder) >> 8))) >> 8;
        t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

        switch (region)
        {
            case 0:
                rgb.r = v;
                rgb.g = t;
                rgb.b = p;
                break;
            case 1:
                rgb.r = q;
                rgb.g = v;
                rgb.b = p;
                break;
            case 2:
                rgb.r = p;
                rgb.g = v;
                rgb.b = t;
                break;
            case 3:
                rgb.r = p;
                rgb.g = q;
                rgb.b = v;
                break;
            case 4:
                rgb.r = t;
                rgb.g = p;
                rgb.b = v;
                break;
            default:
                rgb.r = v;
                rgb.g = p;
                rgb.b = q;
                break;
        }
    }
}

int compareImages(int *_in, int *_out, size_t image_size)
{
    for (size_t i = 0, cnt = 0 ; i < image_size ; i++)
    {
        int in  = _in[i];
        int out = _out[i];
        in  = in  & 0xffffff;
        out = out & 0xffffff;
        if (in != out ){
            cnt++;
            std::cout << "ERROR: Pixel=" << i << " mismatch Expected=" 
                << in << " and Got=" << out << std::endl;
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

