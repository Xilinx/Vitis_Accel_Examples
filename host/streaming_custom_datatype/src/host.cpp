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
#include "ap_int.h"
#include "bitmap.h"
#include "cmdlineparser.h"
#include "rgb_to_hsv.h"
#include "xcl2.hpp"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

// This extension file is required for stream APIs
#include "CL/cl_ext_xilinx.h"
// This file is required for OpenCL C++ wrapper APIs
#include "xcl2.hpp"

// Declaration of custom stream APIs that binds to Xilinx Streaming APIs.
decltype(&clCreateStream) xcl::Stream::createStream = nullptr;
decltype(&clReleaseStream) xcl::Stream::releaseStream = nullptr;
decltype(&clReadStream) xcl::Stream::readStream = nullptr;
decltype(&clWriteStream) xcl::Stream::writeStream = nullptr;
decltype(&clPollStreams) xcl::Stream::pollStreams = nullptr;

// Utility Function Declaration
void sw_RgbToHsv(int *in, int *out, size_t image_size);
void sw_HsvToRgb(int *in, int *out, size_t image_size);
int compareImages(int *in, int *out, size_t image_size);

int main(int argc, char **argv) {
  // Command Line Parser
  sda::utils::CmdLineParser parser;

  // Switches
  //**************//"<Full Arg>",  "<Short Arg>", "<Description>", "<Default>"
  parser.addSwitch("--xclbin_file", "-x", "input binary file string", "");
  parser.addSwitch("--input_file", "-i", "input test data flie", "");
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
    std::cerr << "ERROR:Unable to Read Bitmap File " << bitmapFilename.data()
              << std::endl;
    return EXIT_FAILURE;
  }

  // Allocate Memory in Host Memory
  auto image_size = image.numPixels();
  auto image_size_bytes = sizeof(int) * image_size;
  std::vector<int, aligned_allocator<int>> hwRgbImage(image_size);
  std::vector<int, aligned_allocator<int>> hwHsvImage(image_size);
  std::vector<int, aligned_allocator<int>> swHsvImage(image_size);
  std::vector<int, aligned_allocator<int>> outRgbImage(image_size);

  // Allocate Structure Memory in Host Memory
  auto image_size_bytes_struct = sizeof(RGBcolor) * image_size;
  std::vector<RGBcolor, aligned_allocator<RGBcolor>> hwRgbImagestruct(
      image_size);
  std::vector<HSVcolor, aligned_allocator<HSVcolor>> hwHsvImagestruct(
      image_size);

  // Copying image host buffer
  memcpy(hwRgbImage.data(), image.bitmap(), image_size_bytes);

  // Extracting r, g, b values from ap_int pixel
  for (unsigned int i = 0; i < image_size; i++) {
    ap_int<32> d = hwRgbImage[i];
    hwRgbImagestruct[i].r = d.range(7, 0);
    hwRgbImagestruct[i].g = d.range(15, 8);
    hwRgbImagestruct[i].b = d.range(23, 16);
  }

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
    OCL_CHECK(err, context = cl::Context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(err, q = cl::CommandQueue(context, device,
                                        CL_QUEUE_PROFILING_ENABLE, &err));

    std::cout << "Trying to program device[" << i
              << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
    cl::Program program(context, {device}, bins, NULL, &err);
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

  auto platform_id = device.getInfo<CL_DEVICE_PLATFORM>(&err);

  // Initialization of streaming class is needed before using it.
  xcl::Stream::init(platform_id);

  // Streams
  // Device Connection specification of the stream through extension pointer
  cl_int ret;
  cl_mem_ext_ptr_t ext;
  ext.param = krnl_rgb2hsv.get();
  ext.obj = NULL;

  // Create write stream for argument 0 of kernel
  cl_stream rgb_stream, hsv_stream;
  ext.flags = 0;
  OCL_CHECK(ret, rgb_stream = xcl::Stream::createStream(device.get(),
                                                        XCL_STREAM_READ_ONLY,
                                                        CL_STREAM, &ext, &ret));

  // Create write stream for argument 1 of kernel
  ext.flags = 1;
  OCL_CHECK(ret, hsv_stream = xcl::Stream::createStream(device.get(),
                                                        XCL_STREAM_WRITE_ONLY,
                                                        CL_STREAM, &ext, &ret));

  // Launch the Kernel
  cl::Event b_wait_event;
  OCL_CHECK(err, err = q.enqueueTask(krnl_rgb2hsv, NULL, &b_wait_event));

  // Initiating the WRITE transfer
  cl_stream_xfer_req b_wr_req{0};

  b_wr_req.flags = CL_STREAM_EOT;
  b_wr_req.priv_data = reinterpret_cast<void *>(const_cast<char *>("b_rgb"));

  // Thread 1 for writing data to input stream 1 independently in case of
  // default blocking transfers.
  std::thread thr1{[&] {
    err = OCL_CHECK(
        ret, xcl::Stream::writeStream(rgb_stream, hwRgbImagestruct.data(),
                                      image_size_bytes_struct, &b_wr_req, &ret))
  }};

  // Initiating the READ transfer
  cl_stream_xfer_req b_rd_req{0};
  b_rd_req.flags = CL_STREAM_EOT;
  b_rd_req.priv_data = reinterpret_cast<void *>(const_cast<char *>("b_hsv"));
  // Output thread to read the stream data independently in case of default
  // blocking transfers.
  std::thread thr2{[&] {
    err = OCL_CHECK(
        ret, xcl::Stream::readStream(hsv_stream, hwHsvImagestruct.data(),
                                     image_size_bytes_struct, &b_rd_req, &ret))
  }};

  // Waiting for all the threads to complete their respective operations.
  thr1.join();
  thr2.join();

  // Ensuring all OpenCL objects are released.
  q.finish();

  // Accumulating h, s, v to form ap_int pixel
  for (unsigned int i = 0; i < image_size; i++)
    hwHsvImage[i] =
        (ap_int<8>(hwHsvImagestruct[i].h), ap_int<8>(hwHsvImagestruct[i].s),
         ap_int<8>(hwHsvImagestruct[i].v));

  // Calculating sw based HSV Image
  sw_RgbToHsv(image.bitmap(), swHsvImage.data(), image.numPixels());

  // Compare the results of the Device to the Sw Based
  int match =
      compareImages(swHsvImage.data(), hwHsvImage.data(), image.numPixels());

  // Converting Generated HSV to back to RGB and Writing RGB file to disk
  sw_HsvToRgb(hwHsvImage.data(), outRgbImage.data(), image.numPixels());
  image.writeBitmapFile(outRgbImage.data());

  std::cout << "TEST " << (match ? "FAILED" : "PASSED") << std::endl;
  return match ? EXIT_FAILURE : EXIT_SUCCESS;
}

// Utility Functions Definitions Start Here

// Convert RGB to HSV Format
void sw_RgbToHsv(int *in, int *out, size_t image_size) {
  RGBcolor rgb;
  HSVcolor hsv;
  for (size_t i = 0; i < image_size; i++) {
    rgb.r = in[i] & 0xff;
    rgb.g = (in[i] & 0xff00) >> 8;
    rgb.b = (in[i] & 0xff0000) >> 16;

    auto rgbMin = std::min(rgb.r, std::min(rgb.g, rgb.b));
    auto rgbMax = std::max(rgb.r, std::max(rgb.g, rgb.b));

    hsv.v = rgbMax;
    // if rgbMax = 0
    if (hsv.v == 0) {
      hsv.h = 0;
      hsv.s = 0;
      hsv.v = rgbMax;
    } else {
      hsv.s = 255 * ((long)(rgbMax - rgbMin)) / hsv.v;
      // if hsv.s = 0 => rgbMax - rgbMin = 0
      if (hsv.s == 0)
        hsv.h = 0;
      // if rgbMax is equal to rgb.r then compute hsv.h
      else if (rgbMax == rgb.r)
        hsv.h = 0 + 43 * (rgb.g - rgb.b) / (rgbMax - rgbMin);
      // if rgbMax is equal to rgb.g then compute hsv.h
      else if (rgbMax == rgb.g)
        hsv.h = 85 + 43 * (rgb.b - rgb.r) / (rgbMax - rgbMin);
      // if rgbMax is equal to rgb.b then compute hsv.h
      else
        hsv.h = 171 + 43 * (rgb.r - rgb.g) / (rgbMax - rgbMin);
    }
    out[i] = hsv.h | (hsv.s << 8) | (hsv.v << 16);
  }
}

// Convert RGB to HSV Format
void sw_HsvToRgb(int *in, int *out, size_t image_size) {
  for (size_t i = 0; i < image_size; i++) {
    HSVcolor hsv;
    hsv.h = in[i] & 0xff;
    hsv.s = (in[i] & 0xff00) >> 8;
    hsv.v = (in[i] & 0xff0000) >> 16;

    RGBcolor rgb;

    if (hsv.s == 0) {
      rgb.r = hsv.v;
      rgb.g = hsv.v;
      rgb.b = hsv.v;
    } else {
      // converting to 32 bit to prevent overflow
      unsigned int h = hsv.h;
      unsigned int s = hsv.s;
      unsigned int v = hsv.v;

      unsigned int remainder = (h % 43) * 6;

      unsigned char p = (v * (255 - s)) >> 8;
      unsigned char q = (v * (255 - ((s * remainder) >> 8))) >> 8;
      unsigned char t = (v * (255 - ((s * (255 - remainder)) >> 8))) >> 8;

      unsigned char region = h / 43;

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
    out[i] = rgb.r | (rgb.g << 8) | (rgb.b << 16);
  }
}

int compareImages(int *_in, int *_out, size_t image_size) {
  for (size_t i = 0, cnt = 0; i < image_size; i++) {
    int in = _in[i];
    int out = _out[i];
    in = in & 0xffffff;
    out = out & 0xffffff;
    if (in != out) {
      cnt++;
      std::cerr << "ERROR: Pixel=" << i << " mismatch Expected=" << in
                << " and Got=" << out << std::endl;
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}
