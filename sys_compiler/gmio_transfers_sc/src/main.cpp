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

#include "acc.hpp"
#include <cstdlib>
#include <cstring>

void init_arrays(int si, float* A, float* X, int sz) {
    for (int i = 0; i < sz; i++) {
        A[i] = si + 0.01 * i;
        X[i] = -1.1;
    }
}

bool acc_check_result(int si, float* X, int sz) {
    for (int i = 0; i < sz; i++) {
        float Ai = si + 0.01 * i;
        float Xi = Ai + 0.0001 * i;
        if (X[i] != Xi) {
            printf("ERROR: mismatch si=%d, i=%d, X[i]=%.19f!=%.19f\n", si, i, X[i], Xi);
            return false;
        }
    }
    return true;
}

template <class ACC>
bool run(int sz, int num) {
    const char* acc = typeid(ACC).name();
    while (*acc >= '0' && *acc <= '9') acc++;
    printf("RUNNING: %s\n", acc);
    bool pass = true;
    auto ibp = ACC::create_bufpool(vpp::input);
    auto obp = ACC::create_bufpool(vpp::output);
    int si = 0;
    ACC::send_while([&]() -> bool {
        float* A = (float*)ACC::alloc_buf(ibp, sz * sizeof(float));
        float* X = (float*)ACC::alloc_buf(obp, sz * sizeof(float));
        init_arrays(si, A, X, sz);
        ACC::compute(A, X, sz);
        ACC::set_handle(si);
        return ++si < num;
    });
    ACC::receive_all_in_order([&]() {
        int ri = ACC::get_handle();
        float* X = (float*)ACC::get_buf(obp);
        pass &= acc_check_result(ri, X, sz);
    });
    ACC::join();
    printf("%s %s\n", acc, pass ? "PASSED" : "FAILED");
    return pass;
}

int main(int argc, char* argv[]) {
    int sz = 32;
    int num = 10;
    if (const char* env_var = std::getenv("XCL_EMULATION_MODE")) {
        if (strcmp(env_var, "hw_emu") == 0) {
            sz = 3;
            num = 1;
        }
    }

    bool pass = true;
    pass &= run<acc_seq>(sz, num);
    pass &= run<acc_rnd>(sz, num);
    pass &= run<acc_zc>(sz, num);

    printf("TEST %s\n", pass ? "PASSED" : "FAILED");
}
