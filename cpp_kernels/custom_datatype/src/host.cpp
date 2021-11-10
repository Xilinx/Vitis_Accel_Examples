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
#include "cmdlineparser.h"
#include "xcl2.hpp"
#include <vector>
#define USE_IN_HOST
#include "bitmap.h"
#include "rgb_to_hsv.h"

// Utility Function Declaration
void sw_RgbToHsv(int* in, int* out, size_t image_size);
void sw_HsvToRgb(int* in, int* out, size_t image_size);
int compareImages(int* in, int* out, size_t image_size);

int main(int argc, char** argv) {
    // Command Line Parser
    sda::utils::CmdLineParser parser;

    // Switches
    //**************//"<Full Arg>",  "<Short Arg>", "<Description>", "<Default>"
    parser.addSwitch("--xclbin_file", "-x", "input binary file string", "");
    parser.addSwitch("--input_file", "-i", "input test data file", "");
    parser.parse(argc, argv);

    // Read settings
    std::string binaryFile = parser.value("xclbin_file");
    std::string bitmapFilename = parser.value("input_file");

    if (argc != 5) {
        parser.printHelp();
        return EXIT_FAILURE;
    }

    cl_int err;
    cl::CommandQueue q;
    cl::Context context;
    cl::Kernel krnl_rgb2hsv;
    // Read the bit map file into memory
    BitmapInterface image(bitmapFilename.data());
    bool result = image.readBitmapFile();
    if (!result) {
        std::cerr << "ERROR:Unable to Read Bitmap File " << bitmapFilename.data() << std::endl;
        return EXIT_FAILURE;
    }

    // Allocate Memory in Host Memory
    auto image_size = image.numPixels();
    size_t image_size_bytes = sizeof(int) * image_size;
    std::vector<int, aligned_allocator<int> > hwRgbImage(image_size);
    std::vector<int, aligned_allocator<int> > hwHsvImage(image_size);
    std::vector<int, aligned_allocator<int> > swHsvImage(image_size);
    std::vector<int, aligned_allocator<int> > outRgbImage(image_size);

    // Copying image host buffer
    memcpy(hwRgbImage.data(), image.bitmap(), image_size_bytes);

    // OPENCL HOST CODE AREA START
    auto devices = xcl::get_xil_devices();
    auto device = devices[0];
    // read_binary_file() is a utility API which will load the binaryFile
    // and will return the pointer to file buffer.
    auto fileBuf = xcl::read_binary_file(binaryFile);
    cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    bool valid_device = false;
    for (unsigned int i = 0; i < devices.size(); i++) {
        auto device = devices[i];
        // Creating Context and Command Queue for selected Device
        OCL_CHECK(err, context = cl::Context(device, nullptr, nullptr, nullptr, &err));
        OCL_CHECK(err, q = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err));

        std::cout << "Trying to program device[" << i << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
        cl::Program program(context, {device}, bins, nullptr, &err);
        if (err != CL_SUCCESS) {
            std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
        } else {
            std::cout << "Device[" << i << "]: program successful!\n";
            OCL_CHECK(err, krnl_rgb2hsv = cl::Kernel(program, "rgb_to_hsv", &err));
            valid_device = true;
            break; // we break because we found a valid device
        }
    }
    if (!valid_device) {
        std::cerr << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }

    // Allocate Buffer in Global Memory
    OCL_CHECK(err, cl::Buffer buffer_rgbImage(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, image_size_bytes,
                                              hwRgbImage.data(), &err));

    OCL_CHECK(err, cl::Buffer buffer_hsvImage(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, image_size_bytes,
                                              hwHsvImage.data(), &err));

    OCL_CHECK(err, err = krnl_rgb2hsv.setArg(0, buffer_rgbImage));
    OCL_CHECK(err, err = krnl_rgb2hsv.setArg(1, buffer_hsvImage));
    OCL_CHECK(err, err = krnl_rgb2hsv.setArg(2, image_size));

    // Copy input RGB Image to device global memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_rgbImage}, 0 /* 0 means from host*/));

    // Launch the Kernel
    OCL_CHECK(err, err = q.enqueueTask(krnl_rgb2hsv));

    // Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_hsvImage}, CL_MIGRATE_MEM_OBJECT_HOST));
    OCL_CHECK(err, err = q.finish());

    // OPENCL HOST CODE AREA END

    // Calculating sw based HSV Image
    sw_RgbToHsv(image.bitmap(), swHsvImage.data(), image.numPixels());

    // Compare the results of the Device to the Sw Based
    int match = compareImages(swHsvImage.data(), hwHsvImage.data(), image.numPixels());

    // Converting Generated HSV to back to RGB and Writing RGB file to disk
    sw_HsvToRgb(hwHsvImage.data(), outRgbImage.data(), image.numPixels());
    image.writeBitmapFile(outRgbImage.data());

    std::cout << "TEST " << (match ? "FAILED" : "PASSED") << std::endl;
    return (match ? EXIT_FAILURE : EXIT_SUCCESS);
}

// Utility Functions Definitions Start Here

// Convert RGB to HSV Format
void sw_RgbToHsv(int* in, int* out, size_t image_size) {
    RGBcolor rgb;
    HSVcolor hsv;
    for (size_t i = 0; i < image_size; out[i] = hsv.h | (hsv.s << 8) | (hsv.v << 16), i++) {
        rgb.r = (in[i]) & 0xff;
        rgb.g = ((in[i]) & 0xff00) >> 8;
        rgb.b = ((in[i]) & 0xff0000) >> 16;
        unsigned char rgbMin, rgbMax;

        rgbMin = imin(rgb.r, (imin(rgb.g, rgb.b)));
        rgbMax = imax(rgb.r, (imax(rgb.g, rgb.b)));

        hsv.v = rgbMax;
        if (hsv.v == 0) {
            hsv.h = 0;
            hsv.s = 0;
            continue;
        }

        hsv.s = 255 * ((long)(rgbMax - rgbMin)) / hsv.v;
        if (hsv.s == 0) {
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

// Convert RGB to HSV Format
void sw_HsvToRgb(int* in, int* out, size_t image_size) {
    RGBcolor rgb;
    HSVcolor hsv;
    for (size_t i = 0; i < image_size; out[i] = rgb.r | (rgb.g << 8) | (rgb.b << 16), i++) {
        hsv.h = in[i] & 0xff;
        hsv.s = (in[i] & 0xff00) >> 8;
        hsv.v = (in[i] & 0xff0000) >> 16;
        unsigned char region, p, q, t;
        unsigned int h, s, v, remainder;

        if (hsv.s == 0) {
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

        switch (region) {
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

int compareImages(int* _in, int* _out, size_t image_size) {
    for (size_t i = 0, cnt = 0; i < image_size; i++) {
        int in = _in[i];
        int out = _out[i];
        in = in & 0xffffff;
        out = out & 0xffffff;
        if (in != out) {
            cnt++;
            std::cerr << "ERROR: Pixel=" << i << " mismatch Expected=" << in << " and Got=" << out << std::endl;
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}
