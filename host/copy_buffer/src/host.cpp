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
#include "xcl2.hpp"
#include <vector>

using std::vector;

static const int DATA_SIZE = 1024;
static const std::string error_message =
    "Error: Result mismatch:\n"
    "i = %d CPU result = %d Device result = %d\n";

// This example illustrates the simple OpenCL example that performs
// buffer copy from one buffer to another
int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];
    cl_int err;
    cl::Context context;
    cl::Kernel kernel;
    cl::CommandQueue q;
    size_t size_in_bytes = DATA_SIZE * sizeof(int);

    vector<int, aligned_allocator<int> > source_a(DATA_SIZE, 13);
    vector<int, aligned_allocator<int> > source_results(DATA_SIZE);

    auto devices = xcl::get_xil_devices();
    auto fileBuf = xcl::read_binary_file(binaryFile);
    cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    bool valid_device = false;
    for (unsigned int i = 0; i < devices.size(); i++) {
        auto device = devices[i];
        auto device_name = device.getInfo<CL_DEVICE_NAME>();
        if (xcl::is_hw_emulation()) {
            if (device_name.find("2018") != std::string::npos) {
                std::cout << "[INFO]: The example is not supported for " << device_name
                          << " for hw_emu. Please try other flows." << '\n';
                return EXIT_SUCCESS;
            }
        }
        if (device_name.find("u50_gen3x16_xdma_2019") != std::string::npos) {
            std::cout << "[INFO]: The example runs on non-CDMA mode for " << device_name << '\n';
        }
        // Creating Context and Command Queue for selected Device
        OCL_CHECK(err, context = cl::Context(device, nullptr, nullptr, nullptr, &err));
        OCL_CHECK(err, q = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err));

        std::cout << "Trying to program device[" << i << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
        cl::Program program(context, {device}, bins, nullptr, &err);
        if (err != CL_SUCCESS) {
            std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
        } else {
            std::cout << "Device[" << i << "]: program successful!\n";
            OCL_CHECK(err, kernel = cl::Kernel(program, "vector_add", &err));
            valid_device = true;
            break; // we break because we found a valid device
        }
    }
    if (!valid_device) {
        std::cout << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }

    OCL_CHECK(err, cl::Buffer buffer_a(context, CL_MEM_READ_ONLY, size_in_bytes, nullptr, &err));
    OCL_CHECK(err, cl::Buffer buffer_result(context, CL_MEM_WRITE_ONLY, size_in_bytes, nullptr, &err));
    OCL_CHECK(err, cl::Buffer buffer_b(context, CL_MEM_READ_ONLY, size_in_bytes, nullptr, &err));

    OCL_CHECK(err, err = kernel.setArg(0, buffer_result));
    OCL_CHECK(err, err = kernel.setArg(1, buffer_a));
    OCL_CHECK(err, err = kernel.setArg(2, buffer_b));
    OCL_CHECK(err, err = kernel.setArg(3, DATA_SIZE));

    // copy buffer a to device.
    OCL_CHECK(err, err = q.enqueueWriteBuffer(buffer_a, CL_TRUE, 0, size_in_bytes, source_a.data(), nullptr, nullptr));

    OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_a}, 0 /* 0 means from host*/));

    // This enqueueCopyBuffer() command will copy buffer from buffer_a to buffer_b
    OCL_CHECK(err, err = q.enqueueCopyBuffer(buffer_a, buffer_b, 0, 0, size_in_bytes));

    // This function will execute the kernel on the FPGA
    OCL_CHECK(err, err = q.enqueueNDRangeKernel(kernel, 0, 1, 1, nullptr, nullptr));

    OCL_CHECK(err, err = q.enqueueReadBuffer(buffer_result, CL_TRUE, 0, size_in_bytes, source_results.data(), nullptr,
                                             nullptr));
    OCL_CHECK(err, err = q.finish());

    int match = 0;
    printf("Result = \n");
    for (int i = 0; i < DATA_SIZE; i++) {
        int host_result = source_a[i] + source_a[i];
        if (source_results[i] != host_result) {
            printf(error_message.c_str(), i, host_result, source_results[i]);
            match = 1;
            break;
        } else {
            printf("%d ", source_results[i]);
            if (((i + 1) % 16) == 0) printf("\n");
        }
    }

    std::cout << "TEST " << (match ? "FAILED" : "PASSED") << std::endl;
    return (match ? EXIT_FAILURE : EXIT_SUCCESS);
}
