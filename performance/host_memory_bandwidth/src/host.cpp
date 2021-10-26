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

#include "xcl2.hpp"
#include <CL/cl_ext_xilinx.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <xclbin>" << std::endl;
        return EXIT_FAILURE;
    }

    cl_int err;
    cl::Context context;
    cl::CommandQueue q;
    cl::Kernel krnl, krnl_read, krnl_write;
    std::string binaryFile = argv[1];

    // The get_xil_devices will return vector of Xilinx Devices
    auto devices = xcl::get_xil_devices();

    auto fileBuf = xcl::read_binary_file(binaryFile);

    cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    bool valid_device = false;
    for (unsigned int i = 0; i < devices.size(); i++) {
        auto device = devices[i];
        // Creating Context and Command Queue for selected Device
        OCL_CHECK(err, context = cl::Context(device, nullptr, nullptr, nullptr, &err));
        OCL_CHECK(err, q = cl::CommandQueue(context, device,
                                            CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &err));

        std::cout << "Trying to program device[" << i << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
        cl::Program program(context, {device}, bins, nullptr, &err);
        if (err != CL_SUCCESS) {
            std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
        } else {
            std::cout << "Device[" << i << "]: program successful!\n";
            OCL_CHECK(err, krnl = cl::Kernel(program, "bandwidth", &err));
            OCL_CHECK(err, krnl_read = cl::Kernel(program, "read_bandwidth", &err));
            OCL_CHECK(err, krnl_write = cl::Kernel(program, "write_bandwidth", &err));
            valid_device = true;
            break; // we break because we found a valid device
        }
    }
    if (!valid_device) {
        std::cout << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }

    double concurrent_max = 0;
    double read_max = 0;
    double write_max = 0;

    for (size_t i = 4 * 1024; i <= 256 * 1024 * 1024; i *= 2) {
        size_t iter = 1024;
        size_t bufsize = i;

        if (xcl::is_emulation()) {
            iter = 2;
            if (bufsize > 8 * 1024) break;
        }

        /* Input buffer */
        unsigned char* input_host = ((unsigned char*)malloc(bufsize));
        if (input_host == nullptr) {
            std::cout << "Error: Failed to allocate host side copy of OpenCL source "
                      << "buffer of size " << bufsize << std::endl;
            return EXIT_FAILURE;
        }

        for (size_t i = 0; i < bufsize; i++) {
            input_host[i] = i % 256;
        }
        cl::Buffer* buffer[2];

        /* Host mem flags */
        cl_mem_ext_ptr_t input_buffer_ext;
        input_buffer_ext.flags = XCL_MEM_EXT_HOST_ONLY;
        input_buffer_ext.obj = nullptr;
        input_buffer_ext.param = 0;

        cl_mem_ext_ptr_t output_buffer_ext;
        output_buffer_ext.flags = XCL_MEM_EXT_HOST_ONLY;
        output_buffer_ext.obj = nullptr;
        output_buffer_ext.param = 0;

        OCL_CHECK(err, buffer[0] = new cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, bufsize,
                                                  &input_buffer_ext, &err));
        OCL_CHECK(err, buffer[1] = new cl::Buffer(context, CL_MEM_READ_WRITE | CL_MEM_EXT_PTR_XILINX, bufsize,
                                                  &output_buffer_ext, &err));

        OCL_CHECK(err, err = krnl.setArg(0, *(buffer[0])));
        OCL_CHECK(err, err = krnl.setArg(1, *(buffer[1])));
        OCL_CHECK(err, err = krnl.setArg(2, bufsize));
        OCL_CHECK(err, err = krnl.setArg(3, iter));

        double dbytes = bufsize;
        std::string size_str = xcl::convert_size(bufsize);

        /* Write input buffer */
        /* Map input buffer for PCIe write */
        unsigned char* map_input_buffer0;
        OCL_CHECK(err, map_input_buffer0 = (unsigned char*)q.enqueueMapBuffer(
                           *(buffer[0]), CL_FALSE, CL_MAP_WRITE_INVALIDATE_REGION, 0, bufsize, nullptr, nullptr, &err));
        OCL_CHECK(err, err = q.finish());

        /* prepare data to be written to the device */
        for (size_t i = 0; i < bufsize; i++) {
            map_input_buffer0[i] = input_host[i];
        }
        OCL_CHECK(err, err = q.enqueueUnmapMemObject(*(buffer[0]), map_input_buffer0));

        OCL_CHECK(err, err = q.finish());

        /* Execute Kernel */
        auto start = std::chrono::high_resolution_clock::now();
        q.enqueueTask(krnl);
        q.finish();
        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        double msduration = duration / iter;

        /* Copy results back from OpenCL buffer */
        unsigned char* map_output_buffer0;
        OCL_CHECK(err, map_output_buffer0 = (unsigned char*)q.enqueueMapBuffer(*(buffer[1]), CL_FALSE, CL_MAP_READ, 0,
                                                                               bufsize, nullptr, nullptr, &err));
        OCL_CHECK(err, err = q.finish());

        /* Check the results of output0 */
        for (size_t i = 0; i < bufsize; i++) {
            if (map_output_buffer0[i] != input_host[i]) {
                std::cout << "ERROR : kernel failed to copy entry " << i << " input " << input_host[i] << " output "
                          << map_output_buffer0[i] << std::endl;
                return EXIT_FAILURE;
            }
        }

        /* Profiling information */
        double dsduration = msduration / ((double)1000000);

        double bpersec = (dbytes / dsduration);
        double gbpersec = (2 * bpersec) / ((double)1024 * 1024 * 1024); // For Concurrent Read and Write

        std::cout << "Concurrent Read and Write Throughput = " << gbpersec << " (GB/sec) for buffer size " << size_str
                  << std::endl;

        if (gbpersec > concurrent_max) {
            concurrent_max = gbpersec;
        }

        OCL_CHECK(err, err = krnl_read.setArg(0, *(buffer[0])));
        OCL_CHECK(err, err = krnl_read.setArg(1, bufsize));
        OCL_CHECK(err, err = krnl_read.setArg(2, iter));

        /* Execute Kernel */
        auto read_start = std::chrono::high_resolution_clock::now();
        q.enqueueTask(krnl_read);
        q.finish();
        auto read_end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(read_end - read_start).count();
        msduration = duration / iter;

        /* Profiling information */
        dsduration = msduration / ((double)1000000);

        bpersec = (dbytes / dsduration);
        gbpersec = bpersec / ((double)1024 * 1024 * 1024);

        std::cout << "Read Throughput = " << gbpersec << " (GB/sec) for buffer size " << size_str << std::endl;

        if (gbpersec > read_max) {
            read_max = gbpersec;
        }

        OCL_CHECK(err, err = krnl_write.setArg(0, *(buffer[1])));
        OCL_CHECK(err, err = krnl_write.setArg(1, bufsize));
        OCL_CHECK(err, err = krnl_write.setArg(2, iter));

        /* Execute Kernel */
        auto write_start = std::chrono::high_resolution_clock::now();
        q.enqueueTask(krnl_write);
        q.finish();
        auto write_end = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::microseconds>(write_end - write_start).count();
        msduration = duration / iter;

        /* Profiling information */
        dsduration = msduration / ((double)1000000);

        bpersec = (dbytes / dsduration);
        gbpersec = bpersec / ((double)1024 * 1024 * 1024);

        std::cout << "Write Throughput = " << gbpersec << " (GB/sec) for buffer size " << size_str << "\n\n";

        if (gbpersec > write_max) {
            write_max = gbpersec;
        }

        delete (buffer[0]);
        delete (buffer[1]);
    }

    std::cout << "Maximum bandwidth achieved :\n";
    std::cout << "Concurrent Read and Write Throughput = " << concurrent_max << " (GB/sec) \n";
    std::cout << "Read Throughput = " << read_max << " (GB/sec) \n";
    std::cout << "Write Throughput = " << write_max << " (GB/sec) \n\n";
    std::cout << "TEST PASSED\n";
    return EXIT_SUCCESS;
}
