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

#include "common.h"
#include "hls_stream.h"
#include "conv_filter_acc.hpp"

struct window {
    U8 pix[FILTER_V_SIZE][FILTER_H_SIZE];
};

void Window2D(unsigned short width, unsigned short height, U8* pixel_stream, hls::stream<window>& window_stream) {
    // Line buffers - used to store [FILTER_V_SIZE-1] entire lines of pixels
    U8 LineBuffer[FILTER_V_SIZE - 1][MAX_IMAGE_WIDTH];
#pragma HLS ARRAY_PARTITION variable = LineBuffer dim = 1 complete
#pragma HLS DEPENDENCE variable = LineBuffer inter false
#pragma HLS DEPENDENCE variable = LineBuffer intra false

    // Sliding window of [FILTER_V_SIZE][FILTER_H_SIZE] pixels
    window Window;

    unsigned col_ptr = 0;
    unsigned ramp_up = width * ((FILTER_V_SIZE - 1) / 2) + (FILTER_H_SIZE - 1) / 2;
    unsigned num_pixels = width * height;
    unsigned num_iterations = num_pixels + ramp_up;

    const unsigned max_iterations =
        MAX_IMAGE_WIDTH * MAX_IMAGE_HEIGHT + MAX_IMAGE_WIDTH * ((FILTER_V_SIZE - 1) / 2) + (FILTER_H_SIZE - 1) / 2;

// Iterate until all pixels have been processed
update_window:
    for (int n = 0; n < num_iterations; n++) {
#pragma HLS LOOP_TRIPCOUNT max = max_iterations
#pragma HLS PIPELINE II = 1

        // Read a new pixel from the input stream
        U8 new_pixel = (n < num_pixels) ? pixel_stream[n] : 0;

        // Shift the window and add a column of new pixels from the line buffer
        for (int i = 0; i < FILTER_V_SIZE; i++) {
            for (int j = 0; j < FILTER_H_SIZE - 1; j++) {
                Window.pix[i][j] = Window.pix[i][j + 1];
            }
            Window.pix[i][FILTER_H_SIZE - 1] = (i < FILTER_V_SIZE - 1) ? LineBuffer[i][col_ptr] : new_pixel;
        }

        // Shift pixels in the column of pixels in the line buffer, add the newest pixel
        for (int i = 0; i < FILTER_V_SIZE - 2; i++) {
            LineBuffer[i][col_ptr] = LineBuffer[i + 1][col_ptr];
        }
        LineBuffer[FILTER_V_SIZE - 2][col_ptr] = new_pixel;

        // Update the line buffer column pointer
        if (col_ptr == (width - 1)) {
            col_ptr = 0;
        } else {
            col_ptr++;
        }

        // Write output only when enough pixels have been read the buffers and ramped-up
        if (n >= ramp_up) {
            window_stream.write(Window);
        }
    }
}

void Filter2D(unsigned short width,
              unsigned short height,
              float factor,
              short bias,
              char* coeff_stream,
              hls::stream<window>& window_stream,
              U8* pixel_stream) {
    assert(width <= MAX_IMAGE_WIDTH);
    assert(height <= MAX_IMAGE_HEIGHT);

    // Filtering coefficients
    char coeffs[FILTER_V_SIZE][FILTER_H_SIZE];
#pragma HLS ARRAY_PARTITION variable = coeffs complete dim = 0

// Load the coefficients into local storage
load_coefs:
    for (int i = 0; i < FILTER_V_SIZE; i++) {
        for (int j = 0; j < FILTER_H_SIZE; j++) {
#pragma HLS PIPELINE II = 1
            coeffs[i][j] = coeff_stream[i * FILTER_H_SIZE + j];
        }
    }

// Process the incoming stream of pixel windows
apply_filter:
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
#pragma HLS PIPELINE II = 1
            // Read a 2D window of pixels
            window w = window_stream.read();

            // Apply filter to the 2D window
            int sum = 0;
            for (int row = 0; row < FILTER_V_SIZE; row++) {
                for (int col = 0; col < FILTER_H_SIZE; col++) {
                    unsigned char pixel;
                    int xoffset = (x + col - (FILTER_H_SIZE / 2));
                    int yoffset = (y + row - (FILTER_V_SIZE / 2));
                    // Deal with boundary conditions : clamp pixels to 0 when outside of image
                    if ((xoffset < 0) || (xoffset >= width) || (yoffset < 0) || (yoffset >= height)) {
                        pixel = 0;
                    } else {
                        pixel = w.pix[row][col];
                    }
                    sum += pixel * (char)coeffs[row][col];
                }
            }

            // Normalize result
            unsigned char outpix = MIN(MAX((int(factor * sum) + bias), 0), 255);

            // Write the output pixel
            pixel_stream[y * width + x] = outpix;
        }
    }
}

void conv_acc::krnl_conv(char* coeffs,
                         float factor,
                         short bias,
                         unsigned short width,
                         unsigned short height,
                         unsigned char* src,
                         unsigned char* dst) {
#pragma HLS DATAFLOW

    hls::stream<window, 3> window_stream; // Set FIFO depth to 0 to minimize resources

    // Read incoming pixels and form valid HxV windows
    Window2D(width, height, src, window_stream);

    // Process incoming stream of pixels, and stream pixels out
    Filter2D(width, height, factor, bias, coeffs, window_stream, dst);
}

void conv_acc::compute(char* coeffs,
                       float factor,
                       short bias,
                       unsigned short width,
                       unsigned short height,
                       unsigned char* src,
                       unsigned char* dst) {
    krnl_conv(coeffs, factor, bias, width, height, src, dst);
}
