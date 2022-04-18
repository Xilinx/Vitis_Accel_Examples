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
#include "filter.hpp"

/// For num_iter times:
/// Create an integer input stream of inSz random values between zero (incl) and dice (excl)
/// The compute will return an output stream with all zero values filtered out
///
/// We should only copy back from the device the exact amount (outSz) of filtered data
///
void filter(int inSz, int dice, int numIter) {
    // setenv("VPP_SC_IOB", "0", 0);
    int* golden = new int[numIter * 2];
    int* insave = new int[numIter * inSz];

    auto inBP = xfilter0::create_bufpool(vpp::input);
    auto tmpBP = xfilter0::create_bufpool(vpp::remote);
    auto tmpSzBP = xfilter0::create_bufpool(vpp::output);
    auto outBP = xfilter0::create_bufpool(vpp::output);
    auto outSzBP = xfilter0::create_bufpool(vpp::output);
    srand(inSz + (dice << 2) + (numIter << 4));
    xfilter0::send_while([=]() -> bool {
        static int iter = 0;
        int* in = xfilter0::alloc_buf<int>(inBP, inSz);
        int* tmp = xfilter0::alloc_buf<int>(tmpBP, inSz);
        int* tmpSz = xfilter0::alloc_buf<int>(tmpSzBP, 1);

        int cnt0 = 0, cnt1 = 0;
        for (int i = 0; i < inSz; i++) {
            in[i] = rand() % dice;
            if (in[i] > 0) ++cnt0;
            if (in[i] > 1) ++cnt1;
            insave[iter * inSz + i] = in[i];
        }
        golden[iter * 2 + 0] = cnt0;
        golden[iter * 2 + 1] = cnt1;
        xfilter0::compute(in, inSz, tmp, tmpSz);
        return (++iter < numIter);
    });
    xfilter1::send_while([=]() -> bool {
        static int iter0 = 0;
        int* tmp;
        int tmpSz;
        bool cond = xfilter0::receive_one_in_order([=, &tmp, &tmpSz]() {
            tmp = xfilter0::transfer_buf<int>(tmpBP);
            int* ptrSz = xfilter0::get_buf<int>(tmpSzBP);
            // note ptrSz buffer will get reused by xfiler0::send_while at exit of this receive_one body
            tmpSz = *ptrSz;
        });
        if (!cond) return false;
        int* out = xfilter1::alloc_buf<int>(outBP, tmpSz);
        int* outSz = xfilter1::alloc_buf<int>(outSzBP, 1);
        printf("%3d: tmpSz = %d, golden0 = %d\n", iter0, tmpSz, golden[iter0 * 2 + 0]);
        assert(tmpSz == golden[iter0 * 2 + 0]);

        xfilter1::custom_sync_outputs([=]() {
            xfilter1::sync_output<int>(outSz, 1).get();
            xfilter1::sync_output<int>(out, outSz[0]);
        });
        xfilter1::compute(tmp, tmpSz, out, outSz);
        iter0++;
        return true;
    });
    xfilter1::receive_all_in_order([=]() {
        static int iter1 = 0;
        int* out = xfilter1::get_buf<int>(outBP);
        int* outSz = xfilter1::get_buf<int>(outSzBP);
        printf("%3d: outSz = %d, golden1 = %d\n", iter1, *outSz, golden[iter1 * 2 + 1]);
        if (*outSz != golden[iter1 * 2 + 1]) {
            printf("ERROR: 3d: outSz(%d) != golden1(%d)\n", iter1, *outSz, golden[iter1 * 2 + 1]);
        }
        int i = 0, j = 0;
        for (; i < inSz && j < *outSz; i++, j++) {
            while (insave[iter1 * inSz + i] <= 1) i++; // 0's and 1's filtered out
            if (insave[iter1 * inSz + i] != out[j]) {
                printf("ERROR: %d: in[%d](%d) != out[%d](%d)\n", iter1, i, insave[iter1 * inSz + i], j, out[j]);
                static int mm = 10;
                if (mm-- == 0) abort();
            }
        }
        while (i < inSz && insave[iter1 * inSz + i] <= 1) i++; // 0's and 1's filtered out
        if (i < inSz || j < *outSz) {
            printf("ERROR: %d: inSz=%d, outSz=%d\n", iter1, i, j);
        }
        iter1++;
    });
    xfilter0::join();
    xfilter1::join();
    printf("TESTCASE PASSED\n");
}

int main(int argc, const char** argv) {
    int inSz = 100;
    int dice = 6;
    int numIter = 10;
    filter(inSz, dice, numIter);
}
