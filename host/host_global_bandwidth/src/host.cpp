/**
* Copyright (C) 2020 Xilinx, Inc
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

#include <CL/opencl.h>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>

#include "xcl2.hpp"

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

static int host_to_dev(cl::CommandQueue commands, int buff_size, std::vector<cl::Memory>& mems, std::ostream& strm) {
    cl_int err;
    Timer timer;
    OCL_CHECK(err, err = commands.enqueueMigrateMemObjects(mems, 0 /* 0 means from host*/));

    commands.finish();

    double timer_stop2 = timer.stop();
    double throput = (double)(buff_size * mems.size());
    throput *= 1000000;     // convert us to s;
    throput /= 1024 * 1024; // convert to MB
    throput /= timer_stop2;
    double dbuff_size = (double)(buff_size) / 1024; // convert to KB
    std::cout << "OpenCL migration BW host to device: " << throput << " MB/s"
              << " for buffer size " << dbuff_size << " KB with " << mems.size() << " buffers\n";
    strm << "Host to Card, " << dbuff_size << " KB, " << mems.size() << ", " << throput << "\n";

    if (throput > throput_max_host_to_dev[0]) {
        throput_max_host_to_dev[0] = throput;
        throput_max_host_to_dev[1] = dbuff_size;
        throput_max_host_to_dev[2] = mems.size();
    }
    return CL_SUCCESS;
}

static int dev_to_host(cl::CommandQueue commands, int buff_size, std::vector<cl::Memory>& mems, std::ostream& strm) {
    cl_int err;
    Timer timer;
    OCL_CHECK(err, err = commands.enqueueMigrateMemObjects(mems, CL_MIGRATE_MEM_OBJECT_HOST));

    commands.finish();

    long long timer_stop2 = timer.stop();
    double throput = (double)(buff_size * mems.size());
    throput *= 1000000;     // convert us to s;
    throput /= 1024 * 1024; // convert to MB
    throput /= timer_stop2;
    double dbuff_size = (double)(buff_size) / 1024; // convert to KB
    std::cout << "OpenCL migration BW device to host: " << throput << " MB/s"
              << " for buffer size " << dbuff_size << " KB with " << mems.size() << " buffers\n";
    strm << "Card to Host, " << dbuff_size << " KB, " << mems.size() << ", " << throput << "\n";
    if (throput > throput_max_dev_to_host[0]) {
        throput_max_dev_to_host[0] = throput;
        throput_max_dev_to_host[1] = dbuff_size;
        throput_max_dev_to_host[2] = mems.size();
    }
    return CL_SUCCESS;
}

static int bidirectional(cl::CommandQueue commands,
                         int buff_size,
                         std::vector<cl::Memory>& mems1,
                         std::vector<cl::Memory>& mems2,
                         std::ostream& strm) {
    cl_int err;
    // Writing to avoid read-without-write case in DDR
    OCL_CHECK(err, err = commands.enqueueMigrateMemObjects(mems2, 0 /* 0 means from host*/));
    commands.finish();

    Timer timer;
    OCL_CHECK(err, err = commands.enqueueMigrateMemObjects(mems1, 0 /* 0 means from host*/));
    OCL_CHECK(err, err = commands.enqueueMigrateMemObjects(mems2, CL_MIGRATE_MEM_OBJECT_HOST));

    commands.finish();

    long long timer_stop2 = timer.stop();
    double throput = (double)(buff_size * (mems1.size() + mems2.size()));
    throput *= 1000000;     // convert us to s;
    throput /= 1024 * 1024; // convert to MB
    throput /= timer_stop2;
    double dbuff_size = (double)(buff_size) / 1024; // convert to KB
    std::cout << "OpenCL migration BW "
              << "overall: " << throput << " MB/s for buffer size " << dbuff_size << " KB with " << mems1.size()
              << " buffers\n";
    strm << "Card to Host, " << dbuff_size << " KB, " << mems1.size() << ", " << throput << "\n";

    if (throput > throput_max_bidirectional[0]) {
        throput_max_bidirectional[0] = throput;
        throput_max_bidirectional[1] = dbuff_size;
        throput_max_bidirectional[2] = mems1.size();
    }
    return CL_SUCCESS;
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

    cl_int err;
    cl::Context context;
    cl::CommandQueue command_queue;
    cl::Kernel krnl_bandwidth;
    // The get_xil_devices will return vector of Xilinx Devices
    auto devices = xcl::get_xil_devices();

    // read_binary() command will find the OpenCL binary file
    auto fileBuf = xcl::read_binary_file(binaryFile);
    cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    bool valid_device = false;
    for (unsigned int i = 0; i < devices.size(); i++) {
        auto device = devices[i];
        // Creating Context and Command Queue for selected Device
        OCL_CHECK(err, context = cl::Context(device, nullptr, nullptr, nullptr, &err));
        OCL_CHECK(err, command_queue = cl::CommandQueue(
                           context, device, CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &err));

        std::cout << "Trying to program device[" << i << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
        cl::Program program(context, {device}, bins, nullptr, &err);
        if (err != CL_SUCCESS) {
            std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
        } else {
            std::cout << "Device[" << i << "]: program successful!\n";
            OCL_CHECK(err, krnl_bandwidth = cl::Kernel(program, "bandwidth", &err));
            valid_device = true;
            break; // we break because we found a valid device
        }
    }
    if (!valid_device) {
        std::cout << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }

    const int dim1 = sizeof(buff_tab) / (2 * 4);

    std::ofstream handle("metric1.csv");
    handle << "Direction, Buffer Size (bytes), Count, Bandwidth (MB/s)\n";

    for (int buff_size_1 = 0; buff_size_1 < dim1; buff_size_1++) {
        int nxtcnt = buff_tab[buff_size_1][0];
        int buff_cnt = buff_tab[buff_size_1][1];
        std::vector<cl::Memory> mems(buff_cnt);
        cl_mem_ext_ptr_t bufExt[buff_cnt];

        for (int i = buff_cnt - 1; i >= 0; i--) {
            bufExt[i].flags = XCL_MEM_DDR_BANK1;
            bufExt[i].obj = nullptr;
            bufExt[i].param = 0;
            OCL_CHECK(err, mems[i] = cl::Buffer(context, (cl_mem_flags)(CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX),
                                                nxtcnt, &bufExt[i], &err));
            OCL_CHECK(err, err = command_queue.enqueueFillBuffer<int>((cl::Buffer&)mems[i], i, 0, nxtcnt, 0, 0));
        }

        if (err != CL_SUCCESS) {
            break;
        }

        command_queue.finish();

        err = host_to_dev(command_queue, nxtcnt, mems, handle);
        if (err != CL_SUCCESS) {
            break;
        }

        err = dev_to_host(command_queue, nxtcnt, mems, handle);
        if (err != CL_SUCCESS) {
            break;
        }
    }

    printf("\nThe bandwidth numbers for bidirectional case:\n");
    for (int buff_size_1 = 0; buff_size_1 < dim1; buff_size_1++) {
        int nxtcnt = buff_tab[buff_size_1][0];
        int buff_cnt = buff_tab[buff_size_1][1];
        std::vector<cl::Memory> mems1(buff_cnt);
        std::vector<cl::Memory> mems2(buff_cnt);
        cl_mem_ext_ptr_t bufExt1[buff_cnt];
        cl_mem_ext_ptr_t bufExt2[buff_cnt];

        for (int i = buff_cnt - 1; i >= 0; i--) {
            bufExt1[i].flags = XCL_MEM_DDR_BANK0;
            bufExt1[i].obj = nullptr;
            bufExt1[i].param = 0;

            bufExt2[i].flags = XCL_MEM_DDR_BANK1;
            bufExt2[i].obj = nullptr;
            bufExt2[i].param = 0;

            OCL_CHECK(err, mems1[i] = cl::Buffer(context, (cl_mem_flags)(CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX),
                                                 nxtcnt, &bufExt1[i], &err));
            OCL_CHECK(err, err = command_queue.enqueueFillBuffer<int>((cl::Buffer&)mems1[i], i, 0, nxtcnt, 0, 0));
            OCL_CHECK(err, mems2[i] = cl::Buffer(context, (cl_mem_flags)(CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX),
                                                 nxtcnt, &bufExt2[i], &err));
            OCL_CHECK(err, err = command_queue.enqueueFillBuffer<int>((cl::Buffer&)mems2[i], i, 0, nxtcnt, 0, 0));
        }

        if (err != CL_SUCCESS) {
            break;
        }

        command_queue.finish();
        // printf("\nThe bandwidth numbers for bidirectional case:\n");
        err = bidirectional(command_queue, nxtcnt, mems1, mems2, handle);
        if (err != CL_SUCCESS) {
            break;
        }
    }

    std::cout << "\nMaximum bandwidth achieved :\n";
    std::cout << "OpenCL migration BW host to device: " << throput_max_host_to_dev[0] << " MB/s"
              << " for buffer size " << throput_max_host_to_dev[1] << " KB with " << throput_max_host_to_dev[2]
              << " buffers\n";
    handle << "\nMaximum bandwidth achieved :\n";
    handle << "Host to Card, " << throput_max_host_to_dev[1] << " KB, " << throput_max_host_to_dev[2] << ", "
           << throput_max_host_to_dev[0] << "\n";
    std::cout << "OpenCL migration BW device to host: " << throput_max_dev_to_host[0] << " MB/s"
              << " for buffer size " << throput_max_dev_to_host[1] << " KB with " << throput_max_dev_to_host[2]
              << " buffers\n";
    handle << "Card to Host, " << throput_max_dev_to_host[1] << " KB, " << throput_max_dev_to_host[2] << ", "
           << throput_max_dev_to_host[0] << "\n";
    std::cout << "OpenCL migration BW "
              << "overall: " << throput_max_bidirectional[0] << " MB/s for buffer size " << throput_max_bidirectional[1]
              << " KB with " << throput_max_bidirectional[2] << " buffers\n";
    handle << "Card to Host, " << throput_max_bidirectional[1] << " KB, " << throput_max_bidirectional[2] << ", "
           << throput_max_bidirectional[0] << "\n";

    printf("\nTEST PASSED\n");
    // Shutdown and cleanup
    handle.close();
}
