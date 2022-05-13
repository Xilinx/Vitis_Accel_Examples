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

#include <iostream>
#include "acc.hpp"

// user-defined handle to sync between send/receive threads
class sw_handle // has a limit of 64-bits
{
   public:
    int M;    // data size
    int iter; // compute iteration
};

void initData(float* A, int M);
void VerifyResult(float* A, float* B, float* E, int M, int iter, int printFreq);
void setOptions(int& numTests, int& printFreq, int argc, char* argv[]);

int main(int argc, char* argv[]) {
    int numTests, printFreq;
    setOptions(numTests, printFreq, argc, argv);

    std::cout << "Running " << numTests << " tests (upto " << MaxM << 'x' << MaxM << " matrix each) ...\n";

    // setup accelerator input/output
    auto Abp = pipelined_cu::create_bufpool(vpp::input);
    auto Bbp = pipelined_cu::create_bufpool(vpp::input);
    auto Ebp = pipelined_cu::create_bufpool(vpp::output);

    // Start the send-thread and body (in a lambda function)
    pipelined_cu::send_while([=]() -> bool {
        static int iter = 0;
        sw_handle h;
        h.M = (rand() % MaxM) + 1; // random input size
        h.iter = iter;
        // set the user-defined handle to sync with receive-thread
        pipelined_cu::set_handle<sw_handle>(h);

        // dynamic input size allocation
        float* A = (float*)pipelined_cu::alloc_buf(Abp, h.M * h.M * 4);
        float* B = (float*)pipelined_cu::alloc_buf(Bbp, h.M * h.M * 4);
        float* E = (float*)pipelined_cu::alloc_buf(Ebp, h.M * h.M * 4);

        // setup input data
        initData(A, h.M);
        initData(B, h.M);

        // accelerator call (is non blocking)
        pipelined_cu::compute(A, B, E, h.M);

        return (iter++ < numTests); // when false, send-thread will stop
    });

    // Start the receive-thread and body (in a lambda function)
    pipelined_cu::receive_all_in_order([=]() {
        sw_handle h = pipelined_cu::get_handle<sw_handle>();

        // blocking call on results
        float* E = (float*)pipelined_cu::get_buf(Ebp);
        // get input buffers for comparison (is not blocking)
        float* A = (float*)pipelined_cu::get_buf(Abp);
        float* B = (float*)pipelined_cu::get_buf(Bbp);

        if (!(printFreq == -1)) // disable verify
            VerifyResult(A, B, E, h.M, h.iter, printFreq);
    });

    pipelined_cu::join(); // wait till the threads finish

    std::cout << "Test passed"
              << "\n";
}

// initialize random input data
void initData(float* A, int M) {
    for (int i = 0; i < (M * M); i++) A[i] = rand() % (M * M);
}

// SW computation
void mmult_incr_sw(float* A, float* B, float* C, int M) {
    float A_tmp[MaxM][MaxM], B_tmp[MaxM][MaxM];

    // 1d to 2d arrays
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < M; j++) {
            A_tmp[i][j] = A[i * M + j];
            B_tmp[i][j] = B[i * M + j];
        }
    }
    // compute result
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < M; j++) {
            float result = 0;
            for (int k = 0; k < M; k++) {
                result += A_tmp[i][k] * B_tmp[k][j];
            }
            C[i * M + j] = result;
        }
    }
    for (int i = 0; i < (M * M); i++) {
        C[i] = C[i] + 10 + 20;
    }
}

void dump(std::string s, float* A, int M) {
    std::cout << s;
    for (int i = 0; i < M * M; i++) std::cout << A[i] << " ";
    std::cout << "\n";
}

void VerifyResult(float* A, float* B, float* E, int M, int iter, int printFreq) {
    static float golden_result[MaxM * MaxM];
    if (printFreq) {
        if ((iter % printFreq))
            std::cout << "." << std::flush;
        else
            std::cout << '(' << iter << '/' << M << ')' << std::flush;
    }
    mmult_incr_sw(A, B, golden_result, M);

    for (int i = 0; i < M; i++) {
        if (golden_result[i] != E[i]) {
            std::cout << "\nERROR: Test iter-" << iter << "size-" << M << " Failed: Golden[" << i
                      << "]: " << golden_result[i] << "  Result: " << E[i] << "\n";
            dump("A: ", A, M);
            dump("B: ", B, M);
            exit(1);
        }
    }
}

void setOptions(int& numTests, int& printFreq, int argc, char* argv[]) {
    switch (vpp::flow) {
        case vpp::hw_emu:
            numTests = 12;
            printFreq = 5;
            break;
        default: // case vpp::sw_emu: case vpp::hw:
            numTests = 1002;
            printFreq = 100;
            break;
    }
    // Use "host.exe <numTests> -1" to disable verification when measuring performance
    if (argc > 1) numTests = atoi(argv[1]);
    if (argc > 2) printFreq = atoi(argv[2]);
}
