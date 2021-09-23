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
#define DATA_SIZE (16 * 1024 * 1024)

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <xclbin>" << std::endl;
        return EXIT_FAILURE;
    }

    int dataSize = DATA_SIZE;
    if (xcl::is_emulation()) {
        dataSize = 1024;
    }

    // Compute the size of array in bytes
    uint32_t size_in_bytes = dataSize * sizeof(uint32_t);
    cl_int err;
    cl::Context context;
    cl::CommandQueue q;
    cl::Kernel compute_krnl, copy_krnl;
    std::string binaryFile = argv[1];

    // The get_xil_devices will return vector of Xilinx Devices
    auto devices = xcl::get_xil_devices();

    auto fileBuf = xcl::read_binary_file(binaryFile);

    cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    bool valid_device = false;
    for (unsigned int i = 0; i < devices.size(); i++) {
        auto device = devices[i];
        auto device_name = device.getInfo<CL_DEVICE_NAME>();
        if (xcl::is_hw_emulation()) {
            if (device_name.find("u250_gen3x16_xdma_3_1_202020") != std::string::npos) {
                std::cout << "[INFO]: The example is not supported for u250_gen3x16_xdma_3_1_202020 for hw_emu. Please "
                             "try other flows."
                          << '\n';
                return EXIT_SUCCESS;
            }
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
            OCL_CHECK(err, compute_krnl = cl::Kernel(program, "krnl_vadd", &err));
            OCL_CHECK(err, copy_krnl = cl::Kernel(program, "copy_kernel", &err));
            valid_device = true;
            break; // we break because we found a valid device
        }
    }
    if (!valid_device) {
        std::cout << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }
    cl_mem_ext_ptr_t host_buffer_ext;
    host_buffer_ext.flags = XCL_MEM_EXT_HOST_ONLY;
    host_buffer_ext.obj = nullptr;
    host_buffer_ext.param = 0;

    std::cout << "Creating Buffers " << std::endl;
    OCL_CHECK(err, cl::Buffer buffer_in_host_a(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, size_in_bytes,
                                               &host_buffer_ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_in_host_b(context, CL_MEM_READ_ONLY | CL_MEM_EXT_PTR_XILINX, size_in_bytes,
                                               &host_buffer_ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_out_host(context, CL_MEM_WRITE_ONLY | CL_MEM_EXT_PTR_XILINX, size_in_bytes,
                                              &host_buffer_ext, &err));
    OCL_CHECK(err, cl::Buffer buffer_in_dev_a(context, CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS, size_in_bytes,
                                              nullptr, &err));
    OCL_CHECK(err, cl::Buffer buffer_in_dev_b(context, CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS, size_in_bytes,
                                              nullptr, &err));
    OCL_CHECK(err, cl::Buffer buffer_out_dev(context, CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS, size_in_bytes, nullptr,
                                             &err));

    uint32_t* in_a;
    uint32_t* in_b;
    uint32_t* out;

    int direction = 0;

    std::cout << "Setting copy kernel arguments to copy data from host to device " << std::endl;
    copy_krnl.setArg(0, buffer_in_host_a);
    copy_krnl.setArg(1, buffer_in_dev_a);
    copy_krnl.setArg(2, dataSize);
    copy_krnl.setArg(3, direction);

    OCL_CHECK(err, in_a = (uint32_t*)q.enqueueMapBuffer(buffer_in_host_a, CL_TRUE, CL_MAP_WRITE, 0, size_in_bytes,
                                                        nullptr, nullptr, &err));
    q.finish();

    for (int i = 0; i < dataSize; i++) {
        in_a[i] = i + 1;
    }

    std::cout << "Enqueuing copy kernel to copy data from host to device " << std::endl;
    q.enqueueTask(copy_krnl);
    q.finish();

    copy_krnl.setArg(0, buffer_in_host_b);
    copy_krnl.setArg(1, buffer_in_dev_b);

    OCL_CHECK(err, in_b = (uint32_t*)q.enqueueMapBuffer(buffer_in_host_b, CL_TRUE, CL_MAP_WRITE, 0, size_in_bytes,
                                                        nullptr, nullptr, &err));

    q.finish();

    for (int i = 0; i < dataSize; i++) {
        in_b[i] = (i + 1) * 2;
    }

    q.enqueueTask(copy_krnl);
    q.finish();

    std::cout << "Setting compute kernel arguments " << std::endl;
    compute_krnl.setArg(0, buffer_in_dev_a);
    compute_krnl.setArg(1, buffer_in_dev_b);
    compute_krnl.setArg(2, buffer_out_dev);
    compute_krnl.setArg(3, dataSize);

    direction = 1;

    std::cout << "Setting copy kernel arguments to copy data from device to host " << std::endl;
    copy_krnl.setArg(0, buffer_out_host);
    copy_krnl.setArg(1, buffer_out_dev);
    copy_krnl.setArg(3, direction);

    OCL_CHECK(err, out = (uint32_t*)q.enqueueMapBuffer(buffer_out_host, CL_TRUE, CL_MAP_READ, 0, size_in_bytes, nullptr,
                                                       nullptr, &err));
    q.finish();

    for (int i = 0; i < dataSize; i++) {
        out[i] = 0;
    }

    std::cout << "Enqueuing compute kernel " << std::endl;
    q.enqueueTask(compute_krnl);
    q.finish();

    std::cout << "Enqueuing copy kernel to copy data from device to host " << std::endl;
    q.enqueueTask(copy_krnl);
    q.finish();

    // Compare the results of the Device to the simulation
    bool match = true;
    std::cout << "Comparing device and simulation results " << std::endl;
    for (int i = 0; i < dataSize; i++) {
        if (out[i] != (in_a[i] + in_b[i])) {
            std::cout << "Error: Result mismatch" << std::endl;
            std::cout << "i = " << i << " CPU result = " << in_a[i] + in_b[i] << " Device result = " << out[i]
                      << std::endl;
            match = false;
            break;
        }
    }
    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;
    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
