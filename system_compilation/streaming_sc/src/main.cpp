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
#include "stream_krnl.hpp"

int main() {
    int sz = 32;
    int S = 10;
    printf("VPP_ACC-default : Running %d iterations of sz %d\n", S, sz);
    auto Abp = fsk_acc::create_bufpool(vpp::input);
    auto Xbp = fsk_acc::create_bufpool(vpp::output);
    // sending input
    int si = 0;
    fsk_acc::send_while([&si, sz, S, Abp, Xbp]() -> bool {
        int* A = (int*)fsk_acc::alloc_buf(Abp, sz * sizeof(int));
        int* X = (int*)fsk_acc::alloc_buf(Xbp, sz * sizeof(int));
        for (int i = 0; i < sz; i++) {
            A[i] = si + i;
        }
        fsk_acc::compute(A, X, sz);
        return (++si < S);
    });
    // receiving output
    int ri = 0;
    bool ok = true;
    fsk_acc::receive_all_in_order([&ri, &ok, sz, Xbp]() {
        int* X = (int*)fsk_acc::get_buf(Xbp);
        for (int i = 0; i < sz; i++) {
            if (X[i] != ri + i + 1) {
                ok = false;
            }
        }
        ++ri;
    });
    // join
    fsk_acc::join();
    if (!ok)
        printf("ERROR: mismatch\n");
    else
        printf("testcase PASSED\n");
    return ok ? 0 : 1;
}
