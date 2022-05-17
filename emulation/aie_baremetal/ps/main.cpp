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
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include "platform.h"
#include "xparameters.h"
#include "xil_io.h"
#include "xil_cache.h"
#include "input.h"
#include "golden.h"

#include "graph.cpp"

#define MM2S_BASE XPAR_XMM2S_0_S_AXI_CONTROL_BASEADDR
#define S2MM_BASE XPAR_XS2MM_0_S_AXI_CONTROL_BASEADDR

#define MEM_OFFSET 0x10
#define SIZE_OFFSET 0x1C
#define CTRL_OFFSET 0x0

#define INPUT_SIZE 4096
#define OUTPUT_SIZE 1024

void InitData(int32_t** out, int size) {
    int i;
    *out = (int32_t*)malloc(sizeof(int32_t) * size);

    if (!out) {
        printf("Allocation of memory failed \n");
        exit(-1);
    }

    for (i = 0; i < size; i++) {
        (*out)[i] = 0xABCDEF00;
    }
}
int RunTest(uint64_t mm2s_base,
            uint64_t s2mm_base,
            int32_t* in,
            int32_t* golden,
            int32_t* out,
            int input_size,
            int output_size) {
    int i;
    int errCount = 0;
    uint64_t memAddr = (uint64_t)in;
    uint64_t mem_outAddr = (uint64_t)out;

    printf("Starting test w/ cu\n");

    Xil_Out32(mm2s_base + MEM_OFFSET, (uint32_t)memAddr);
    Xil_Out32(mm2s_base + MEM_OFFSET + 4, 0);
    Xil_Out32(s2mm_base + MEM_OFFSET, (uint32_t)mem_outAddr);
    Xil_Out32(s2mm_base + MEM_OFFSET + 4, 0);
    Xil_Out32(mm2s_base + SIZE_OFFSET, input_size);
    Xil_Out32(s2mm_base + SIZE_OFFSET, output_size);
    Xil_Out32(mm2s_base + CTRL_OFFSET, 1);
    Xil_Out32(s2mm_base + CTRL_OFFSET, 1);

    printf("GRAPH INIT\n");
    clipgraph.init();

    printf("GRAPH RUN\n");
    clipgraph.run();

    while (1) {
        uint32_t v = Xil_In32(s2mm_base + CTRL_OFFSET);
        if (v & 6) {
            break;
        }
    }

    printf("PLIO IP DONE!\n");

    for (i = 0; i < output_size; i++) {
        if ((((int32_t*)out)[i] != ((int32_t*)golden)[i])) {
            printf("Error found in sample %d != to the golden %d\n", i + 1, ((int32_t*)out)[i], ((int32_t*)golden)[i]);
            errCount++;
        } else
            printf("%d\n ", ((int32_t*)out)[i]);
    }

    printf("Ending test w/ cu\n");
    return errCount;
}

int main() {
    int i;
    int32_t* out;
    int errCount;

    Xil_DCacheDisable();

    sleep(1);
    printf("- \n");
    printf("Beginning test\n");

    InitData(&out, OUTPUT_SIZE);

    errCount = RunTest(MM2S_BASE, S2MM_BASE, (int32_t*)cint16input, int32golden, out, INPUT_SIZE, OUTPUT_SIZE);

    if (errCount == 0)
        printf("Test passed. \n");
    else
        printf("ERROR: Test failed! Error count: %d \n", errCount);

    return errCount;
}
