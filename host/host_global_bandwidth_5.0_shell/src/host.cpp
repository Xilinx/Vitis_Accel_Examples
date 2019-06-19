/**********
Copyright (c) 2018, Xilinx, Inc.
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation
and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software
without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**********/

#include <CL/opencl.h>
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <vector>

#include "xcl2.hpp"

////////////////////////////////////////////////////////////////////////////////
class Timer {
    std::chrono::high_resolution_clock::time_point mTimeStart;

  public:
    Timer() { reset(); }
    long long stop() {
        std::chrono::high_resolution_clock::time_point timeEnd =
            std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(timeEnd -
                                                                     mTimeStart)
            .count();
    }
    void reset() { mTimeStart = std::chrono::high_resolution_clock::now(); }
};

static int host_to_dev(cl::CommandQueue commands,
                       int buff_size,
                       std::vector<cl::Memory> &mems,
                       std::ostream &strm) {
    cl_int err;
    Timer timer;
    OCL_CHECK(err,
              err = commands.enqueueMigrateMemObjects(
                  mems, 0 /* 0 means from host*/));

    commands.finish();

    double timer_stop2 = timer.stop();
    double throput = (double)(buff_size * mems.size());
    throput *= 1000000;     // convert us to s;
    throput /= 1024 * 1024; // convert to MB
    throput /= timer_stop2;
    double dbuff_size = (double)(buff_size) / 1024; // convert to KB
    std::cout << "OpenCL migration BW host to device: " << throput << " MB/s"
              << " for buffer size " << dbuff_size << " KB with " << mems.size()
              << " buffers\n";
    strm << "Host to Card, " << dbuff_size << " KB, " << mems.size() << ", "
         << throput << "\n";
    return CL_SUCCESS;
}

static int dev_to_host(cl::CommandQueue commands,
                       int buff_size,
                       std::vector<cl::Memory> &mems,
                       std::ostream &strm) {
    cl_int err;
    Timer timer;
    OCL_CHECK(err,
              err = commands.enqueueMigrateMemObjects(
                  mems, CL_MIGRATE_MEM_OBJECT_HOST));

    commands.finish();

    long long timer_stop2 = timer.stop();
    double throput = (double)(buff_size * mems.size());
    throput *= 1000000;     // convert us to s;
    throput /= 1024 * 1024; // convert to MB
    throput /= timer_stop2;
    double dbuff_size = (double)(buff_size) / 1024; // convert to KB
    std::cout << "OpenCL migration BW device to host: " << throput << " MB/s"
              << " for buffer size " << dbuff_size << " KB with " << mems.size()
              << " buffers\n";
    strm << "Card to Host, " << dbuff_size << " KB, " << mems.size() << ", "
         << throput << "\n";
    return CL_SUCCESS;
}

static int bidirectional(cl::CommandQueue commands,
                         int buff_size,
                         std::vector<cl::Memory> &mems1,
                         std::vector<cl::Memory> &mems2,
                         std::ostream &strm) {
    cl_int err;
    //Writing to avoid read-without-write case in DDR
    OCL_CHECK(err,
              err = commands.enqueueMigrateMemObjects(
                  mems2, 0 /* 0 means from host*/));
    commands.finish();

    Timer timer;
    OCL_CHECK(err,
              err = commands.enqueueMigrateMemObjects(
                  mems1, 0 /* 0 means from host*/));
    OCL_CHECK(err,
              err = commands.enqueueMigrateMemObjects(
                  mems2, CL_MIGRATE_MEM_OBJECT_HOST));

    commands.finish();

    long long timer_stop2 = timer.stop();
    double throput = (double)(buff_size * (mems1.size() + mems2.size()));
    throput *= 1000000;     // convert us to s;
    throput /= 1024 * 1024; // convert to MB
    throput /= timer_stop2;
    double dbuff_size = (double)(buff_size) / 1024; // convert to KB
    std::cout << "OpenCL migration BW "
              << " overall:" << throput << " MB/s for buffer size "
              << dbuff_size << " KB with " << mems1.size() << " buffers\n";
    strm << "Card to Host, " << dbuff_size << " KB, " << mems1.size() << ", "
         << throput << "\n";
    return CL_SUCCESS;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];

    //Variable-------------------------------------------------------------------------------

    int buff_tab[][2] = {{64, 1024},
                         {256, 1024},
                         {512, 1024},
                         {1024, 1024},
                         {4096, 1024},
                         {16384, 512},
                         {1048576, 8},
                         {1048576, 64},
                         {1048576, 256},
                         {2097152, 8},
                         {2097152, 64},
                         {2097152, 256},
                         {16777216, 64},
                         {268435456, 4},
                         {536870912, 2}};

    cl_int err;
    unsigned fileBufSize;
    // The get_xil_devices will return vector of Xilinx Devices
    auto devices = xcl::get_xil_devices();
    auto device = devices[0];

    //Creating Context and Command Queue for selected Device
    OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(
        err,
        cl::CommandQueue command_queue(
            context,
            device,
            CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE,
            &err));
    OCL_CHECK(err,
              std::string device_name = device.getInfo<CL_DEVICE_NAME>(&err));
    std::cout << "Found Device=" << device_name.c_str() << std::endl;

    // read_binary() command will find the OpenCL binary file
    auto fileBuf = xcl::read_binary_file(binaryFile, fileBufSize);
    cl::Program::Binaries bins{{fileBuf, fileBufSize}};
    devices.resize(1);
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
    OCL_CHECK(err, cl::Kernel krnl_bandwidth(program, "bandwidth", &err));

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
            bufExt[i].obj = NULL;
            bufExt[i].param = 0;
            OCL_CHECK(err,
                      mems[i] =
                          cl::Buffer(context,
                                     (cl_mem_flags)(CL_MEM_ALLOC_HOST_PTR |
                                                    CL_MEM_READ_WRITE |
                                                    CL_MEM_EXT_PTR_XILINX),
                                     nxtcnt,
                                     &bufExt[i],
                                     &err));
            OCL_CHECK(err,
                      err = command_queue.enqueueFillBuffer<int>(
                          (cl::Buffer &)mems[i], i, 0, nxtcnt, 0, 0));
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
            bufExt1[i].obj = NULL;
            bufExt1[i].param = 0;

            bufExt2[i].flags = XCL_MEM_DDR_BANK1;
            bufExt2[i].obj = NULL;
            bufExt2[i].param = 0;

            OCL_CHECK(err,
                      mems1[i] =
                          cl::Buffer(context,
                                     (cl_mem_flags)(CL_MEM_ALLOC_HOST_PTR |
                                                    CL_MEM_READ_WRITE |
                                                    CL_MEM_EXT_PTR_XILINX),
                                     nxtcnt,
                                     &bufExt1[i],
                                     &err));
            OCL_CHECK(err,
                      err = command_queue.enqueueFillBuffer<int>(
                          (cl::Buffer &)mems1[i], i, 0, nxtcnt, 0, 0));
            OCL_CHECK(err,
                      mems2[i] =
                          cl::Buffer(context,
                                     (cl_mem_flags)(CL_MEM_ALLOC_HOST_PTR |
                                                    CL_MEM_READ_WRITE |
                                                    CL_MEM_EXT_PTR_XILINX),
                                     nxtcnt,
                                     &bufExt2[i],
                                     &err));
            OCL_CHECK(err,
                      err = command_queue.enqueueFillBuffer<int>(
                          (cl::Buffer &)mems2[i], i, 0, nxtcnt, 0, 0));
        }

        if (err != CL_SUCCESS) {
            break;
        }

        command_queue.finish();
        //printf("\nThe bandwidth numbers for bidirectional case:\n");
        err = bidirectional(command_queue, nxtcnt, mems1, mems2, handle);
        if (err != CL_SUCCESS) {
            break;
        }
    }

    delete[] fileBuf;
    printf("\nTEST PASSED\n");
    // Shutdown and cleanup
    handle.close();
}
