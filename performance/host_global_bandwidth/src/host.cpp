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

#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>

#include "xrt/xrt_device.h"
#include "xrt/xrt_kernel.h"
#include "xrt/xrt_bo.h"
#include "xrt/xrt_hw_context.h"

double throput_max_host_to_dev[3] = {0};
double throput_max_dev_to_host[3] = {0};
double throput_max_bidirectional[3] = {0};

////////////////////////////////////////////////////////////////////////////////
class Timer {
    std::chrono::high_resolution_clock::time_point mTimeStart;

   public:
    Timer() { reset(); }
    long long stop() {
        std::chrono::high_resolution_clock::time_point timeEnd = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(timeEnd - mTimeStart).count();
    }
    void reset() { mTimeStart = std::chrono::high_resolution_clock::now(); }
};

static int host_to_dev(int buff_size, std::vector<xrt::bo>& bos, std::ostream& strm) {
    Timer timer;
    for (auto& bo : bos) {
        bo.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    }

    double timer_stop2 = timer.stop();
    double throput = (double)(buff_size * bos.size());
    throput *= 1000000;     // convert us to s;
    throput /= 1024 * 1024; // convert to MB
    throput /= timer_stop2;
    double dbuff_size = (double)(buff_size) / 1024; // convert to KB
    std::cout << "XRT migration BW host to device: " << throput << " MB/s"
              << " for buffer size " << dbuff_size << " KB with " << bos.size() << " buffers\n";
    strm << "Host to Card, " << dbuff_size << " KB, " << bos.size() << ", " << throput << "\n";

    if (throput > throput_max_host_to_dev[0]) {
        throput_max_host_to_dev[0] = throput;
        throput_max_host_to_dev[1] = dbuff_size;
        throput_max_host_to_dev[2] = bos.size();
    }
    return 0;
}

static int dev_to_host(int buff_size, std::vector<xrt::bo>& bos, std::ostream& strm) {
    Timer timer;
    for (auto& bo : bos) {
        bo.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
    }

    long long timer_stop2 = timer.stop();
    double throput = (double)(buff_size * bos.size());
    throput *= 1000000;     // convert us to s;
    throput /= 1024 * 1024; // convert to MB
    throput /= timer_stop2;
    double dbuff_size = (double)(buff_size) / 1024; // convert to KB
    std::cout << "XRT migration BW device to host: " << throput << " MB/s"
              << " for buffer size " << dbuff_size << " KB with " << bos.size() << " buffers\n";
    strm << "Card to Host, " << dbuff_size << " KB, " << bos.size() << ", " << throput << "\n";
    if (throput > throput_max_dev_to_host[0]) {
        throput_max_dev_to_host[0] = throput;
        throput_max_dev_to_host[1] = dbuff_size;
        throput_max_dev_to_host[2] = bos.size();
    }
    return 0;
}

static int bidirectional(int buff_size,
                         std::vector<xrt::bo>& bos1,
                         std::vector<xrt::bo>& bos2,
                         std::ostream& strm) {
    // Writing to avoid read-without-write case in DDR
    for (auto& bo : bos2) {
        bo.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    }

    Timer timer;
    for (auto& bo : bos1) {
        bo.sync(XCL_BO_SYNC_BO_TO_DEVICE);
    }
    for (auto& bo : bos2) {
        bo.sync(XCL_BO_SYNC_BO_FROM_DEVICE);
    }

    long long timer_stop2 = timer.stop();
    double throput = (double)(buff_size * (bos1.size() + bos2.size()));
    throput *= 1000000;     // convert us to s;
    throput /= 1024 * 1024; // convert to MB
    throput /= timer_stop2;
    double dbuff_size = (double)(buff_size) / 1024; // convert to KB
    std::cout << "XRT migration BW "
              << "overall: " << throput << " MB/s for buffer size " << dbuff_size << " KB with " << bos1.size()
              << " buffers\n";
    strm << "Card to Host, " << dbuff_size << " KB, " << bos1.size() << ", " << throput << "\n";

    if (throput > throput_max_bidirectional[0]) {
        throput_max_bidirectional[0] = throput;
        throput_max_bidirectional[1] = dbuff_size;
        throput_max_bidirectional[2] = bos1.size();
    }
    return 0;
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];

    // Variable-------------------------------------------------------------------------------

    int buff_tab[][2] = {{64, 1024},    {256, 1024},    {512, 1024},    {1024, 1024},   {4096, 1024},
                         {16384, 512},  {1048576, 8},   {1048576, 64},  {1048576, 256}, {2097152, 8},
                         {2097152, 64}, {2097152, 256}, {16777216, 64}, {268435456, 4}, {536870912, 2}};

    // XRT Native API initialization
    xrt::device device(0);
    auto uuid = device.load_xclbin(binaryFile);
    xrt::hw_context hw_ctx(device, uuid);

    std::cout << "Device[0]: program successful!\n";

    xrt::kernel krnl_bandwidth(hw_ctx, "bandwidth");

    int dim1 = sizeof(buff_tab) / (2 * 4);
    const char* xcl_mode = std::getenv("XCL_EMULATION_MODE");
    if (xcl_mode != nullptr) {
        dim1 = 2; // Reducing combinations to run faster in emulation flow
    }

    std::ofstream handle("metric1.csv");
    handle << "Direction, Buffer Size (bytes), Count, Bandwidth (MB/s)\n";

    int err = 0;
    for (int buff_size_1 = 0; buff_size_1 < dim1; buff_size_1++) {
        int nxtcnt = buff_tab[buff_size_1][0];
        int buff_cnt = buff_tab[buff_size_1][1];
        std::vector<xrt::bo> bos(buff_cnt);

        for (int i = buff_cnt - 1; i >= 0; i--) {
            bos[i] = xrt::bo(hw_ctx, nxtcnt, krnl_bandwidth.group_id(0));
            // Fill buffer with value i
            auto bo_map = bos[i].map<int*>();
            for (int j = 0; j < nxtcnt / sizeof(int); j++) {
                bo_map[j] = i;
            }
        }

        err = host_to_dev(nxtcnt, bos, handle);
        if (err != 0) {
            break;
        }

        err = dev_to_host(nxtcnt, bos, handle);
        if (err != 0) {
            break;
        }
    }

    printf("\nThe bandwidth numbers for bidirectional case:\n");
    for (int buff_size_1 = 0; buff_size_1 < dim1; buff_size_1++) {
        int nxtcnt = buff_tab[buff_size_1][0];
        int buff_cnt = buff_tab[buff_size_1][1];
        std::vector<xrt::bo> bos1(buff_cnt);
        std::vector<xrt::bo> bos2(buff_cnt);

        for (int i = buff_cnt - 1; i >= 0; i--) {
            bos1[i] = xrt::bo(hw_ctx, nxtcnt, krnl_bandwidth.group_id(0));
            auto bo1_map = bos1[i].map<int*>();
            for (int j = 0; j < nxtcnt / sizeof(int); j++) {
                bo1_map[j] = i;
            }

            bos2[i] = xrt::bo(hw_ctx, nxtcnt, krnl_bandwidth.group_id(1));
            auto bo2_map = bos2[i].map<int*>();
            for (int j = 0; j < nxtcnt / sizeof(int); j++) {
                bo2_map[j] = i;
            }
        }

        err = bidirectional(nxtcnt, bos1, bos2, handle);
        if (err != 0) {
            break;
        }
    }

    std::cout << "\nMaximum bandwidth achieved :\n";
    std::cout << "XRT migration BW host to device: " << throput_max_host_to_dev[0] << " MB/s"
              << " for buffer size " << throput_max_host_to_dev[1] << " KB with " << throput_max_host_to_dev[2]
              << " buffers\n";
    handle << "\nMaximum bandwidth achieved :\n";
    handle << "Host to Card, " << throput_max_host_to_dev[1] << " KB, " << throput_max_host_to_dev[2] << ", "
           << throput_max_host_to_dev[0] << "\n";
    std::cout << "XRT migration BW device to host: " << throput_max_dev_to_host[0] << " MB/s"
              << " for buffer size " << throput_max_dev_to_host[1] << " KB with " << throput_max_dev_to_host[2]
              << " buffers\n";
    handle << "Card to Host, " << throput_max_dev_to_host[1] << " KB, " << throput_max_dev_to_host[2] << ", "
           << throput_max_dev_to_host[0] << "\n";
    std::cout << "XRT migration BW "
              << "overall: " << throput_max_bidirectional[0] << " MB/s for buffer size " << throput_max_bidirectional[1]
              << " KB with " << throput_max_bidirectional[2] << " buffers\n";
    handle << "Card to Host, " << throput_max_bidirectional[1] << " KB, " << throput_max_bidirectional[2] << ", "
           << throput_max_bidirectional[0] << "\n";

    printf("\nTEST PASSED\n");
    // Shutdown and cleanup
    handle.close();
}
