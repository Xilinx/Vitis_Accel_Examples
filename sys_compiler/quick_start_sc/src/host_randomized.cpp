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

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <iostream>
#include <vector>
#include <chrono>
#include "conv_filter_acc_wrapper.hpp"
#include "cmdlineparser.h"
#include "conv_filter_cpu.hpp"
#include "coefficients.h"
#include "common.h"
#include "types.hpp"

#define RESET "\033[0m"
#define RED "\033[31m"
#define GREEN "\033[32m"

int main(int argc, char** argv) {
    printf("----------------------------------------------------------------------------\n");
    printf("\n");
    printf("Xilinx 2D Filter Example Application (Randomized Input Version) using System Compiler\n");
    printf("\n");

    // ---------------------------------------------------------------------------------
    // Parse command line
    // ---------------------------------------------------------------------------------

    sda::utils::CmdLineParser parser;
    parser.addSwitch("--nruns", "-n", "Number of times to image is processed", "2");
    parser.addSwitch("--width", "-w", "Image width", "1920");
    parser.addSwitch("--height", "-h", "Image height", "1080");
    parser.addSwitch("--filter", "-f", "Filter type (0-6)", "0");
    parser.addSwitch("--compare", "-c", "Compare FPGA and SW performance", "false", true);

    // parse all command line options
    parser.parse(argc, argv);
    unsigned int width = parser.value_to_int("width");
    unsigned int height = parser.value_to_int("height");
    unsigned int numRuns = parser.value_to_int("nruns");
    unsigned int filterType = parser.value_to_int("filter");
    bool comparePerf = parser.value_to_bool("compare");

    if ((width > MAX_IMAGE_WIDTH) || (height > MAX_IMAGE_HEIGHT)) {
        printf("ERROR: Maximum image size is %dx%d\n", MAX_IMAGE_WIDTH, MAX_IMAGE_HEIGHT);
        return -1;
    }
    if (filterType > 6) {
        printf("ERROR: Supported filter type values are [0:6]\n");
        return -1;
    }

    printf("Number of runs    : %d\n", numRuns);
    printf("Image width       : %d\n", width);
    printf("Image height      : %d\n", height);
    printf("Filter type       : %d\n", filterType);
    printf("Compare perf.     : %d\n", comparePerf);
    printf("\n");

    // ---------------------------------------------------------------------------------

    // Read Input image

    unsigned int nbytes = (width * height);

    // Input and output buffers (Y,U,V)
    YUVImage srcImage;
    YUVImage dstImage;
    srcImage.yChannel = (unsigned char*)malloc(nbytes);
    srcImage.uChannel = (unsigned char*)malloc(nbytes);
    srcImage.vChannel = (unsigned char*)malloc(nbytes);
    dstImage.yChannel = (unsigned char*)malloc(nbytes);
    dstImage.uChannel = (unsigned char*)malloc(nbytes);
    dstImage.vChannel = (unsigned char*)malloc(nbytes);

    // Generate random image data
    printf("Generating a random %dx%d input image\n", width, height);
    for (unsigned int i = 0; i < nbytes; i++) {
        srcImage.yChannel[i] = rand();
        srcImage.uChannel[i] = rand();
        srcImage.vChannel[i] = rand();
        dstImage.yChannel[i] = 0;
        dstImage.uChannel[i] = 0;
        dstImage.vChannel[i] = 0;
    }

    // Retrieve filter factor and bias
    float factor = filterFactors[filterType];
    short bias = filterBiases[filterType];

    // ---------------------------------------------------------------------------------
    // Make requests to kernel(s)
    // ---------------------------------------------------------------------------------

    printf("Running FPGA accelerator on %d images\n", numRuns);
    auto fpga_begin = std::chrono::high_resolution_clock::now();

    conv_filter_execute_fpga(filterCoeffs[filterType], factor, bias, width, height, numRuns, srcImage, dstImage);

    auto fpga_end = std::chrono::high_resolution_clock::now();

    // ---------------------------------------------------------------------------------
    // Compute reference results and compare
    // ---------------------------------------------------------------------------------

    if (comparePerf) {
        printf("Running Software version\n");
    }

    // Create output buffers for reference results
    unsigned char* y_ref = (unsigned char*)malloc(nbytes);
    unsigned char* u_ref = (unsigned char*)malloc(nbytes);
    unsigned char* v_ref = (unsigned char*)malloc(nbytes);

    unsigned numRunsSW = comparePerf ? numRuns : 1;

    auto cpu_begin = std::chrono::high_resolution_clock::now();

#pragma omp parallel for num_threads(3)
    for (unsigned int n = 0; n < numRunsSW; n++) {
        // Compute reference results
        Filter2D(filterCoeffs[filterType], factor, bias, width, height, srcImage.yChannel, y_ref);
        Filter2D(filterCoeffs[filterType], factor, bias, width, height, srcImage.uChannel, u_ref);
        Filter2D(filterCoeffs[filterType], factor, bias, width, height, srcImage.vChannel, v_ref);
    }

    auto cpu_end = std::chrono::high_resolution_clock::now();

    printf("Comparing results\n");

    // Compare results
    bool diff = false;
    for (unsigned int y = 0; y < height; y++) {
        for (unsigned int x = 0; x < width; x++) {
            bool fail = false;
            if (dstImage.yChannel[y * width + x] != y_ref[y * width + x]) fail = true;
            if (dstImage.uChannel[y * width + x] != u_ref[y * width + x]) fail = true;
            if (dstImage.vChannel[y * width + x] != v_ref[y * width + x]) fail = true;
            if (fail && !diff) {
                printf("first diff on pixel (%d,%d)\n", x, y);
                printf("expected: (%3d,%3d,%3d)\n", y_ref[y * width + x], u_ref[y * width + x], v_ref[y * width + x]);
                printf("got:      (%3d,%3d,%3d)\n", dstImage.yChannel[y * width + x], dstImage.uChannel[y * width + x],
                       dstImage.vChannel[y * width + x]);
                diff = true;
            }
        }
    }

    if (diff) {
        printf("\n%sTest FAILED: Output has mismatches with reference%s\n", RED, RESET);
    } else {
        printf("\n%sTest PASSED: Output matches reference%s\n", GREEN, RESET);
    }

    // Report performance (if not running in emulation mode)
    if (getenv("XCL_EMULATION_MODE") == NULL) {
        std::chrono::duration<double> fpga_duration = fpga_end - fpga_begin;
        std::chrono::duration<double> cpu_duration = cpu_end - cpu_begin;
        float fpga_throughput = (double)numRuns * 3 * nbytes / fpga_duration.count() / (1024.0 * 1024.0);
        float cpu_throughput = (double)numRuns * 3 * nbytes / cpu_duration.count() / (1024.0 * 1024.0);
        printf("\n");
        printf("FPGA Time         : %10.4f s\n", fpga_duration.count());
        printf("FPGA Throughput   : %10.4f MB/s\n", fpga_throughput);
        if (comparePerf) {
            printf("CPU  Time         : %10.4f s\n", cpu_duration.count());
            printf("CPU  Throughput   : %10.4f MB/s\n", cpu_throughput);
            printf("FPGA Speedup      : %10.4f x\n", cpu_duration.count() / fpga_duration.count());
        }
    }

    printf("----------------------------------------------------------------------------\n");

    return (diff ? 1 : 0);
}
