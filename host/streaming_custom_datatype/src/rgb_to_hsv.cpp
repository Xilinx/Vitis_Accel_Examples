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

#include "rgb_to_hsv.h"
#include "ap_fixed.h"
#include <cmath>
#include <cstring>
#include <hls_stream.h>
#include <iostream>

extern "C" {

void pkt2struct(hls::stream<RGBcolor> &read_stream, hls::stream<pkt> &in,
                hls::stream<bool> eos[2]) {
  // Converting qdma_axis<DWIDTH, 0, 0, 0> type stream to RGBcolor stream
  do {
    pkt a_t = in.read();
    RGBcolor a_temp(a_t.get_data());
    read_stream << a_temp;
    if (a_t.get_last()) {
      eos[0] << true;
      eos[1] << true;
      break;
    }
    eos[0] << false;
    eos[1] << false;
  } while (true);
}

void compute_dataflow(hls::stream<HSVcolor> &write_stream,
                      hls::stream<RGBcolor> &read_stream,
                      hls::stream<bool> &eos) {

  bool t_last;
  RGBcolor rgb;
  HSVcolor hsv;

// Single loop is sufficient to read each RGB pixel from the input stream,
// Converting RGB pixel to HSV Pixel, and writing HSV Pixel Back to output
// stream.
// Auto-pipeline is going to apply pipeline to this loop
rgb2hsv_loop:
  do {
// Loop is marked for pipeline. Compiler will be able to get Loop II=1
// as a result, Kernel will be able to do burst read and burst write.
// Kernel will be performing RGB to HSV conversion per pixel per clock.
#pragma HLS LOOP_TRIPCOUNT min = c_image_size max = c_image_size
    // LOOP TRIPCOUNT is added so that report estimate can provide estimated
    //  latency Information

    read_stream >> rgb;
    eos >> t_last;

    // Getting Minimum and Maximum value in R, G, and B element of Pixel
    auto rgbMin = std::min(rgb.r, std::min(rgb.g, rgb.b));
    auto rgbMax = std::max(rgb.r, std::max(rgb.g, rgb.b));

    // Algorithm to Calculate HSV from RSB
    hsv.v = rgbMax;
    // if rgbMax = 0
    if (rgbMax == 0) {
      hsv.h = 0;
      hsv.s = 0;
      hsv.v = 0;
    } else {
      hsv.s = 255 * ((long)(rgbMax - rgbMin)) / rgbMax;
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

    // Writing computed HSV Pixel value into Write_Stream
    write_stream << hsv;
    if (t_last)
      break;
  } while (true);
}

void struct2pkt(hls::stream<pkt> &out, hls::stream<HSVcolor> &write_stream,
                hls::stream<bool> &eos) {
  bool t_last;
  // Converting HSVcolor stream to qdma_axis<DWIDTH, 0, 0, 0> type stream
  do {
    eos >> t_last;
    pkt t_out;
    HSVcolor tmpout;
    write_stream >> tmpout;
    t_out.set_data(tmpout.packed());
    t_out.set_keep(-1);
    if (t_last)
      t_out.set_last(1);
    else
      t_out.set_last(0);
    out.write(t_out);
    if (t_last)
      break;
  } while (true);
}

void rgb_to_hsv(hls::stream<pkt> &in, hls::stream<pkt> &out) {

#pragma HLS dataflow

  hls::stream<RGBcolor> read_stream("read");
  hls::stream<HSVcolor> write_stream("write");
  hls::stream<bool> eos[2];

#pragma HLS STREAM variable = eos depth = 64

  pkt2struct(read_stream, in, eos);
  compute_dataflow(write_stream, read_stream, eos[0]);
  struct2pkt(out, write_stream, eos[1]);
}
}
