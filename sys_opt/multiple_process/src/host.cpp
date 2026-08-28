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

/*******************************************************************************

Description:
    This is a simple to demonstrate Multi Process Support(MPS) using HLS
kernels.


Limitation:
    Debug and Profile will not function correctly when multiprocess has been
    enabled.
    Emulation flow does not have support for multiprocess.

*******************************************************************************/

#include "multi_krnl.h"
#include <algorithm>
#include <iostream>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

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

bool run_kernel(std::string& binaryFile, int krnl_id) {
    const char* krnl_names[] = {"krnl_vadd", "krnl_vsub", "krnl_vmul"};

    int pid = getpid();
    printf("\n[PID: %d] Start Vector Operation (PARENT PPID: %d)\n", pid, getppid());

    size_t vector_size_bytes = sizeof(int) * LENGTH;
    std::vector<int, aligned_allocator<int> > source_a(LENGTH);
    std::vector<int, aligned_allocator<int> > source_b(LENGTH);
    std::vector<int, aligned_allocator<int> > result_sw(LENGTH);
    std::vector<int, aligned_allocator<int> > result_hw(LENGTH);

    /* Create the test data and run the vector addition locally */
    std::generate(source_a.begin(), source_a.end(), std::rand);
    std::generate(source_b.begin(), source_b.end(), std::rand);
    for (int i = 0; i < LENGTH; i++) {
        result_hw[i] = 0;
        switch (krnl_id) {
            case 0:
                result_sw[i] = source_a[i] + source_b[i];
                break;
            case 1:
                result_sw[i] = source_a[i] - source_b[i];
                break;
            case 2:
                result_sw[i] = source_a[i] * source_b[i];
                break;
            default:
                std::cout << "Kernel ID is unknown!!" << std::endl;
        }
    }

    // XRT HOST CODE AREA START
    auto device = xrt::device(0);
    printf("\n[PID: %d] Read XCLBIN file\n", pid);

    auto uuid = device.load_xclbin(binaryFile);
    xrt::hw_context hw_ctx(device, uuid);
    std::cout << "Device[0]: program successful!\n";
    std::cout << "[PID: " << pid << "] Create a Program and a [ " << krnl_names[krnl_id] << " ] Kernel\n";
    auto krnl = xrt::kernel(hw_ctx, krnl_names[krnl_id]);

    auto buffer_a = xrt::bo(hw_ctx, vector_size_bytes, krnl.group_id(0));
    auto buffer_b = xrt::bo(hw_ctx, vector_size_bytes, krnl.group_id(1));
    auto buffer_c = xrt::bo(hw_ctx, vector_size_bytes, krnl.group_id(2));

    int vector_length = LENGTH;

    /* Copy input vectors to memory */
    printf("\n[PID: %d] Transfer the Input Data to Device\n", pid);
    std::copy(source_a.begin(), source_a.end(), buffer_a.map<int*>());
    std::copy(source_b.begin(), source_b.end(), buffer_b.map<int*>());
    buffer_a.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    buffer_b.sync(XCL_BO_SYNC_BO_TO_DEVICE);

    /* Launch the kernel */
    printf("[PID: %d] Launch Kernel\n", pid);
    auto run = krnl(buffer_a, buffer_b, buffer_c, vector_length);
    run.wait();

    /* Copy result to local buffer */
    printf("[PID: %d] Transfer the Output Data from Device\n", pid);
    buffer_c.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
    std::copy(buffer_c.map<int*>(), buffer_c.map<int*>() + LENGTH, result_hw.begin());
    // XRT HOST CODE AREA END

    /* Compare the results of the kernel to the simulation */
    bool krnl_match = true;

    printf("\n[PID: %d] Checking the Output Data with Golden Results...\n", pid);

    for (int i = 0; i < LENGTH; i++) {
        if (result_sw[i] != result_hw[i]) {
            printf("Error: i = %d CPU result = %d FPGA Result = %d\n", i, result_sw[i], result_hw[i]);
            krnl_match = false;
            break;
        }
    }

    return krnl_match;
}

int main(int argc, char* argv[]) {
    int iter = 3;

    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File> " << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];

    // Setting XCL_MULTIPROCESS_MODE
    std::cout << "Set the env variable for Multi Process Support (MPS)" << std::endl;
    char mps_env[] = "XCL_MULTIPROCESS_MODE=1";
    if (putenv(mps_env) != 0) {
        std::cout << "putenv failed" << std::endl;
    } else
        std::cout << "Env variable: XCL_MULTIPROCESS_MODE: " << getenv("XCL_MULTIPROCESS_MODE") << std::endl;

    bool result = true;

    std::cout << "Now create (" << iter << ") CHILD processes" << std::endl;
    for (int i = 0; i < iter; i++) {
        if (fork() == 0) {
            printf("[CHILD] PID %d from [PARENT] PPID %d\n", getpid(), getppid());
            result = run_kernel(binaryFile, i);
            exit(!(result));
        }
    }

    // Need to wait for all child process to complete
    std::cout << "\n[PID: " << getpid() << "] PARENT WAITS CHILD TO FINISH.\n\n" << std::endl;
    for (int i = 0; i < iter; i++) {
        int status = 0;
        int child = wait(&status);
        std::cout << "[PID: " << getpid() << "] child: " << child << " exited with WIFEXITED: " << WIFEXITED(status)
                  << " and WEXITSTATUS: " << WEXITSTATUS(status) << std::endl;
        if (WEXITSTATUS(status) and WIFEXITED(status)) result = false;
    }

    std::cout << "TEST " << ((result) ? "PASSED" : "FAILED") << std::endl;
    return ((result) ? EXIT_SUCCESS : EXIT_FAILURE);
}
