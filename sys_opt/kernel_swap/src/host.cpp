/**
* Copyright (C) 2019-2021 Xilinx, Inc
* Copyright 2022-2026 Advanced Micro Devices, Inc.
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
#include "cmdlineparser.h"
#include <iostream>
#include <vector>
#include <cstdlib>
#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"
#include "xrt/xrt_bo.h"
#include "xrt/xrt_hw_context.h"

template <typename T>
struct aligned_allocator {
    using value_type = T;
    T* allocate(std::size_t num) {
        void* ptr = nullptr;
        if (posix_memalign(&ptr, 4096, num * sizeof(T))) throw std::bad_alloc();
        return reinterpret_cast<T*>(ptr);
    }
    void deallocate(T* p, std::size_t num) { free(p); }
};

#define LENGTH 1024

int main(int argc, char** argv) {
    // Command Line Parser
    sda::utils::CmdLineParser parser;

#ifdef dfx_device
    const char* xcl_mode = std::getenv("XCL_EMULATION_MODE");
    if (xcl_mode != nullptr && std::string(xcl_mode) == "hw_emu") {
        std::cout << "INFO: This example is not supported for dfx platforms for hw_emu" << std::endl;
        return EXIT_SUCCESS;
    }
#endif

    // Switches
    //**************//"<Full Arg>",  "<Short Arg>", "<Description>", "<Default>"
    parser.addSwitch("--xclbin_file_krnl_mmult", "-x1", "krnl_mmult binary file string", "");
    parser.addSwitch("--xclbin_file_krnl_madd", "-x2", "krnl_madd binary file string", "");
    parser.parse(argc, argv);

    // Read settings
    auto binaryFile1 = parser.value("xclbin_file_krnl_mmult");
    auto binaryFile2 = parser.value("xclbin_file_krnl_madd");

    if (argc != 5) {
        parser.printHelp();
        return EXIT_FAILURE;
    }

    std::vector<int, aligned_allocator<int> > h_a(LENGTH);    // host memory for a vector
    std::vector<int, aligned_allocator<int> > h_b(LENGTH);    // host memory for b vector
    std::vector<int, aligned_allocator<int> > h_temp(LENGTH); // host memory for temp vector
    std::vector<int, aligned_allocator<int> > h_c(LENGTH);    // host memory for c vector

    // Fill our data sets with pattern
    int i = 0;
    for (i = 0; i < LENGTH; i++) {
        h_a[i] = i;
        h_b[i] = i;
        h_temp[i] = 0;
        h_c[i] = 0;
    }

    xrt::device device(0);
    int vector_length = LENGTH;
    bool match = true;

    std::cout << "Device[0]: program successful!\n";

    // The temporary pointer(h_temp) is created mainly for the dynamic platforms,
    // since in the dynamic platforms we will not be able to load a second xclbin
    // unless all the buffers are released before calling load_xclbin a second
    // time in the same process. The code block below is in braces because the
    // objects are automatically released once the block ends
    {
        std::cout << "INFO: loading vmul kernel\n";
        auto uuid1 = device.load_xclbin(binaryFile1);
        xrt::hw_context hw_ctx1(device, uuid1);
        xrt::kernel krnl_vmul(hw_ctx1, "krnl_vmul");

        xrt::bo d_a(hw_ctx1, sizeof(int) * LENGTH, krnl_vmul.group_id(0));
        xrt::bo d_b(hw_ctx1, sizeof(int) * LENGTH, krnl_vmul.group_id(1));
        xrt::bo d_mul(hw_ctx1, sizeof(int) * LENGTH, krnl_vmul.group_id(2));

        auto map_a = d_a.map<int*>();
        auto map_b = d_b.map<int*>();
        for (int i = 0; i < LENGTH; i++) {
            map_a[i] = h_a[i];
            map_b[i] = h_b[i];
        }
        d_a.sync(XCL_BO_SYNC_BO_TO_DEVICE);
        d_b.sync(XCL_BO_SYNC_BO_TO_DEVICE);

        // Execute kernel
        auto run = krnl_vmul(d_a, d_b, d_mul, vector_length);
        run.wait();

        d_mul.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
        auto map_mul = d_mul.map<int*>();
        for (int i = 0; i < LENGTH; i++) {
            h_temp[i] = map_mul[i];
        }

        // Check Results
        for (int i = 0; i < LENGTH; i++) {
            if ((h_a[i] * h_b[i]) != h_temp[i]) {
                printf("ERROR in vmul - %d - a=%d, b=%d, c=%d\n", i, h_a[i], h_b[i], h_c[i]);
                match = false;
                break;
            }
        }
    }

    {
        if (match) {
            std::cout << "INFO: loading vadd_krnl\n";
            auto uuid2 = device.load_xclbin(binaryFile2);
            xrt::hw_context hw_ctx2(device, uuid2);
            xrt::kernel krnl_vadd(hw_ctx2, "krnl_vadd");

            xrt::bo d_temp(hw_ctx2, sizeof(int) * LENGTH, krnl_vadd.group_id(0));
            xrt::bo d_add(hw_ctx2, sizeof(int) * LENGTH, krnl_vadd.group_id(2));

            auto map_temp = d_temp.map<int*>();
            for (int i = 0; i < LENGTH; i++) {
                map_temp[i] = h_temp[i];
            }
            d_temp.sync(XCL_BO_SYNC_BO_TO_DEVICE);

            // Execute kernel
            auto run = krnl_vadd(d_temp, d_temp, d_add, vector_length);
            run.wait();

            d_add.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
            auto map_add = d_add.map<int*>();
            for (int i = 0; i < LENGTH; i++) {
                h_c[i] = map_add[i];
            }

            // Check Results
            for (int i = 0; i < LENGTH; i++) {
                if ((2 * (h_a[i] * h_b[i])) != h_c[i]) {
                    printf("ERROR in vadd - %d - c=%d\n", i, h_c[i]);
                    match = false;
                    break;
                }
            }
        }
    }

    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;
    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
