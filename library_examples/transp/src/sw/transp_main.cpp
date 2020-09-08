/**********
 * Copyright 2019 Xilinx, Inc.
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
 * **********/

#include <stdio.h>
#include <chrono>
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <iomanip>

#include "fpga.hpp"
#include "types.hpp"
#include "gen_bin.hpp"
#include "gen_transp.hpp"
#include "ddrType.hpp"

using namespace std;

typedef chrono::time_point<chrono::high_resolution_clock> TimePointType;

int main(int argc, char** argv) {
    if (argc < 7) {
        printf("ERROR: passed %d arguments instead of %d, exiting\n", argc, 7);
        printf("  Usage:\n    blas_host.exe  blas.xclbin  transp rows cols handle0 handle1/\n");
        printf("    Example: blas_host.exe blas.xclbin transp 256 512 A0 B0\n");
        return EXIT_FAILURE;
    }

    string l_xclbinFile(argv[1]);
    unsigned int l_argIdx = 2;
    string l_opName = argv[l_argIdx++];
    assert(l_opName == "transp");
    unsigned int l_m = atoi(argv[l_argIdx++]);
    unsigned int l_n = atoi(argv[l_argIdx++]);
    unsigned int l_lda = l_n;
    unsigned int l_ldb = l_m;
    MatFormatType l_formatA = xf::blas::DdrMatrixShape::string2format("rm");
    MatFormatType l_formatB = xf::blas::DdrMatrixShape::string2format("cm");
    string l_handleA(argv[l_argIdx++]);
    string l_handleB(argv[l_argIdx++]);

    GenTransp l_transp;
    if (!l_transp.check(l_m, l_n, l_lda, l_ldb, l_formatA, l_formatB)) exit(1);
    ProgramType l_p[BLAS_numKernels][2];
    for (unsigned int nk = 0; nk < BLAS_numKernels; ++nk) {
        l_transp.addInstr(l_p[nk][0], l_m, l_n, l_lda, l_ldb, l_formatA, l_formatB, l_handleA, l_handleB, 0);
        l_transp.addInstr(l_p[nk][1], l_m, l_n, l_lda, l_ldb, l_formatA, l_formatB, l_handleA, l_handleB, 1);

        GenControl l_control;
        for (unsigned int i = 1; i < BLAS_numInstr - 1; ++i) {
            l_control.addInstr(l_p[nk][0], false, true);
            l_control.addInstr(l_p[nk][1], false, true);
        }
        l_control.addInstr(l_p[nk][0], true, false);
        l_control.addInstr(l_p[nk][1], true, false);
    }

    int l_deviceId = 0;
    if (argc > 7) {
        l_deviceId = atoi(argv[7]);
    }

    TimePointType l_tp[10];
    unsigned int l_tpIdx = 0;
    l_tp[l_tpIdx] = chrono::high_resolution_clock::now();

    xf::blas::Fpga l_fpga(l_deviceId);

    if (l_fpga.loadXclbin(l_xclbinFile)) {
        cout << "INFO: created kernels" << endl;
    } else {
        cerr << "ERROR: failed to load " + l_xclbinFile + "\n";
        return EXIT_FAILURE;
    }
    showTimeData("loadXclbin", l_tp[l_tpIdx], l_tp[l_tpIdx + 1]);
    l_tpIdx++;

    for (unsigned int i = 0; i < BLAS_numKernels; ++i) {
        if (!l_fpga.createKernel(i, "blasKernel")) {
            cerr << "ERROR: failed to create kernel " << i << " " << endl;
        }
    }
    showTimeData("create kernels", l_tp[l_tpIdx], l_tp[l_tpIdx + 1]);
    l_tpIdx++;

    xf::blas::MemDesc l_memDesc[BLAS_numKernels];
    for (unsigned int i = 0; i < BLAS_numKernels; ++i) {
        // l_memDesc[i].init(l_memVec[i].size() * sizeof(DdrType) / BLAS_pageSizeBytes, l_memVec[i].data());
        l_memDesc[i] = l_p[i][0].getMemDesc();
        if (!l_fpga.createBufferForKernel(i, l_memDesc[i])) {
            cerr << "ERROR: failed to create buffer for kernel " << i << endl;
        }
    }
    showTimeData("create buffers", l_tp[l_tpIdx], l_tp[l_tpIdx + 1]);
    l_tpIdx++;

    // Transfer data to FPGA
    for (unsigned int i = 0; i < BLAS_numKernels; ++i) {
        if (l_fpga.copyToKernel(i)) {
            cout << "INFO: transferred data to kernel " << i << endl;
        } else {
            cerr << "ERROR: failed to transfer data to kernel" << i << endl;
            return EXIT_FAILURE;
        }
    }
    showTimeData("copy to kernels", l_tp[l_tpIdx], l_tp[l_tpIdx + 1]);
    l_tpIdx++;

    // launch kernels
    for (unsigned int i = 0; i < BLAS_numKernels; ++i) {
        if (l_fpga.callKernel(i)) {
            cout << "INFO: Executed kernel " << i << endl;
        } else {
            cerr << "ERROR: failed to call kernel " << i << endl;
            return EXIT_FAILURE;
        }
    }
    showTimeData("call kernels", l_tp[l_tpIdx], l_tp[l_tpIdx + 1]);
    l_tpIdx++;
    l_fpga.finish();

    // Transfer data back to host
    for (unsigned int i = 0; i < BLAS_numKernels; ++i) {
        if (l_fpga.copyFromKernel(i)) {
            cout << "INFO: Transferred data from kernel" << i << endl;
        } else {
            cerr << "ERROR: failed to transfer data from kernel " << i << endl;
            return EXIT_FAILURE;
        }
    }
    l_fpga.finish();
    showTimeData("copyFromFpga", l_tp[l_tpIdx], l_tp[l_tpIdx + 1]);
    l_tpIdx++;
    showTimeData("total", l_tp[0], l_tp[l_tpIdx]);
    l_tpIdx++;
    showTimeData("subtotalFpga", l_tp[1], l_tp[l_tpIdx]);
    l_tpIdx++; // Host->DDR, kernel, DDR->host

    // check results
    bool l_res = true;
    for (int i = 0; i < BLAS_numKernels; ++i) {
        KargsType l_kargs0;
        KargsOpType l_op0 = l_kargs0.load(l_p[i][0].getBaseInstrAddr(), 0);
        if (l_op0 != KargsType::OpTransp) {
            cerr << "ERROR: operator has been changed by the CU\n";
            return EXIT_FAILURE;
        }
        TranspArgsType l_transpArgs = l_kargs0.getTranspArgs();
        bool l_opOk = l_transp.compare(1e-3, 1e-6, l_p[i][0], l_p[i][1], l_transpArgs);
        l_res = l_res && l_opOk;
    }
    if (!l_res) {
        cout << "ERROR: there are mismatches between outputs and golden reference" << endl;
        return EXIT_FAILURE;
    }
    cout << "Test Pass!" << endl;
    return EXIT_SUCCESS;
}
