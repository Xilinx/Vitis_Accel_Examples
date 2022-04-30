/**
* Copyright (C) 2022 Xilinx, Inc
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
#include <cstring>
#include <fstream>
#include <iostream>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "xrt/xrt_bo.h"
#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"

#define NBYTES_DATA 4

#define WIDTH 8
#define HEIGHT 8
#define SIZE (WIDTH * HEIGHT)

#define SAMPLES 256

int main(int argc, char** argv) {
    const int input_size_in_bytes = SIZE * sizeof(float);
    const int output_size_in_bytes = SIZE * sizeof(float);
    const int input_size_allocated = ((input_size_in_bytes / 4096) + ((input_size_in_bytes % 4096) > 0)) * 4096;
    const int output_size_allocated = ((output_size_in_bytes / 4096) + ((output_size_in_bytes % 4096) > 0)) * 4096;

    if (argc < 2) {
        std::cout << "Usage: host.exe <xclbin>\n";
        return 1;
    }
    auto device = xrt::device(0);
    auto uuid = device.load_xclbin(argv[1]);

    auto aie_kernel = xrt::kernel(device, uuid.get(), "aie_kernel");
    auto out_bo = xrt::bo(device, output_size_allocated, aie_kernel.group_id(2));
    auto out_bomapped = out_bo.map<float*>();
    memset(out_bomapped, 0, output_size_in_bytes);

    auto in_boA = xrt::bo(device, input_size_allocated, aie_kernel.group_id(0));
    auto in_bomappedA = in_boA.map<float*>();
    auto in_boB = xrt::bo(device, input_size_allocated, aie_kernel.group_id(1));
    auto in_bomappedB = in_boB.map<float*>();

    // setting input data
    float* golden = (float*)malloc(output_size_in_bytes);
    for (int i = 0; i < SIZE; i++) {
        in_bomappedA[i] = rand() % SIZE;
        in_bomappedB[i] = rand() % SIZE;
    }
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            golden[i * WIDTH + j] = 0;
            for (int k = 0; k < WIDTH; k++)
                golden[i * WIDTH + j] += in_bomappedA[i * WIDTH + k] * in_bomappedB[k + WIDTH * j];
        }
    }

    in_boA.sync(XCL_BO_SYNC_BO_TO_DEVICE, input_size_in_bytes, 0);
    in_boB.sync(XCL_BO_SYNC_BO_TO_DEVICE, input_size_in_bytes, 0);

    int rval;

    auto run = aie_kernel(in_boA, in_boB, out_bo, input_size_in_bytes, output_size_in_bytes);
    run.wait();

    out_bo.sync(XCL_BO_SYNC_BO_FROM_DEVICE, output_size_in_bytes, 0);

    int match = 0;
    for (int i = 0; i < SIZE; i++) {
        if (out_bomapped[i] != golden[i]) {
            printf(
                "ERROR: Test failed! Error found in sample %d: golden: %f, "
                "hardware: %f\n",
                i, golden[i], out_bomapped[i]);
            match = 1;
            break;
        }
    }

    std::cout << "TEST " << (match ? "FAILED" : "PASSED") << std::endl;
    return (match ? EXIT_FAILURE : EXIT_SUCCESS);
}
