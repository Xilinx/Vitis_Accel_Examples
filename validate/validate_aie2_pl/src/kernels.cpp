/*
 * Copyright 2022 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "kernels.h"

// ifm sub-volume 8*20*2*16
// wts sub-volume 4928
// ofm sub-volume 8*8*4*7

// ifm and wts sync
// ofm async, 4 ifm generates 1 ofm
#ifdef __X86SIM__
#define THREAD_LOCAL thread_local
#else
#define THREAD_LOCAL
#endif

void dummy_conv2d_with_print(input_window<uint8>* ifm, input_window<uint8>* wts, output_window<uint8>* ofm) {
    THREAD_LOCAL static int iteration = 0;

    if (iteration % 4 == 0) {
        window_acquire(ofm);
        printf("window_acquire(ofm)\n");
    }

    uint32* pIfm = (uint32*)ifm->ptr;
    uint32* pWts = (uint32*)wts->ptr;
    uint32* pOfm = (uint32*)ofm->ptr;

    printf("pOfm %d\n", pOfm);

    int ifmSizeIn32 = 8 * 20 * 2 * 16 / 4;
    int wtsSizeIn32 = 4928 / 4;
    int ofmSizeIn32 = 8 * 8 * 4 * 7 / 4;

    if (iteration % 4 == 0) {
        for (int i = 0; i < ofmSizeIn32; i++) pOfm[i] = 0;
    }

    for (int i = 0; i < ofmSizeIn32; i++) pOfm[i] = pOfm[i] + pIfm[i] + pWts[i];

    printf("ifm\n");
    for (int i = 0; i < ifmSizeIn32; i++) printf("%d ", pIfm[i]);
    printf("\n");

    printf("wts\n");
    for (int i = 0; i < wtsSizeIn32; i++) printf("%d ", pWts[i]);
    printf("\n");

    printf("ofm\n");
    for (int i = 0; i < ofmSizeIn32; i++) printf("%d ", pOfm[i]);
    printf("\n");

    iteration++;
    if (iteration % 4 == 0) {
        window_release(ofm);
        printf("window_release(ofm)\n");
    }
}

void dummy_conv2d(input_window<uint8>* ifm, input_window<uint8>* wts, output_window<uint8>* ofm) {
    THREAD_LOCAL static int iteration = 0;

    if (iteration % 4 == 0) window_acquire(ofm);

    uint32* pIfm = (uint32*)ifm->ptr;
    uint32* pWts = (uint32*)wts->ptr;
    uint32* pOfm = (uint32*)ofm->ptr;

    int ifmSizeIn32 = 8 * 20 * 2 * 16 / 4;
    int wtsSizeIn32 = 4928 / 4;
    int ofmSizeIn32 = 8 * 8 * 4 * 7 / 4;

    if (iteration % 4 == 0) {
        for (int i = 0; i < ofmSizeIn32; i++) pOfm[i] = 0;
    }

    for (int i = 0; i < ofmSizeIn32; i++) pOfm[i] = pOfm[i] + pIfm[i] + pWts[i];

    iteration++;
    if (iteration % 4 == 0) window_release(ofm);
}
