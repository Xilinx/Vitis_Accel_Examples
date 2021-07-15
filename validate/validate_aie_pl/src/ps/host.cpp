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

#include <stdlib.h>
#include "xcl2.hpp"
#include "graph.h"

#define SAMPLES 2048

#include "adf/adf_api/XRTConfig.h"
extern "C" {
#include <xaiengine.h>
}

forwardGraph mygraph;

template <typename T>
T* aligned_alloc(std::size_t num) {
    void* ptr = nullptr;
    if (posix_memalign(&ptr, 4096, num * sizeof(T))) {
        throw std::bad_alloc();
    }
    return reinterpret_cast<T*>(ptr);
}

int main(int argc, char** argv) {
    std::string xclbin_path = "kernel.xclbin";

    int32_t* device_input = aligned_alloc<int32_t>(SAMPLES * sizeof(int32_t));
    int32_t* device_output = aligned_alloc<int32_t>(SAMPLES * sizeof(int32_t));

    for (int i = 0; i < SAMPLES; i++) {
        device_input[i] = std::rand();
    }

    std::vector<cl::Device> devices = xcl::get_xil_devices();
    cl::Device device = devices[0];
    cl::Context context(device, NULL, NULL, NULL);
    cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE);
    std::string devName = device.getInfo<CL_DEVICE_NAME>();
    printf("INFO: Found Device=%s\n", devName.c_str());
    auto fileBuf = xcl::read_binary_file(xclbin_path);
    cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    devices.resize(1);
    cl::Program program(context, devices, bins, NULL);
    cl::Kernel kernel_mm2s(program, "mm2s");
    cl::Kernel kernel_s2mm(program, "s2mm");
    std::cout << "INFO: Kernel has been created" << std::endl;

    std::vector<cl_mem_ext_ptr_t> mext_in(2);
    mext_in[0] = {0, device_input, kernel_mm2s()};
    mext_in[1] = {0, device_output, kernel_s2mm()};

    std::vector<cl::Buffer> buffer(2);

    buffer[0] = cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_READ_WRITE,
                           sizeof(int32_t) * SAMPLES, &mext_in[0]);
    buffer[1] = cl::Buffer(context, CL_MEM_EXT_PTR_XILINX | CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                           sizeof(int32_t) * SAMPLES, &mext_in[1]);

    // add buffers to migrate
    std::vector<cl::Memory> init;
    init.push_back(buffer[0]);
    init.push_back(buffer[1]);

    // migrate data from host to device
    q.enqueueMigrateMemObjects(init, CL_MIGRATE_MEM_OBJECT_CONTENT_UNDEFINED, nullptr, nullptr);
    q.finish();

    // Data transfer from host buffer to device buffer
    std::vector<cl::Memory> ob_in;
    std::vector<cl::Memory> ob_out;
    ob_in.push_back(buffer[0]);
    ob_out.push_back(buffer[1]);

    kernel_mm2s.setArg(0, buffer[0]);
    kernel_mm2s.setArg(2, SAMPLES);

    kernel_s2mm.setArg(0, buffer[1]);
    kernel_s2mm.setArg(2, SAMPLES);

    // Setup kernel
    std::cout << "INFO: Finish kernel setup" << std::endl;
    std::vector<cl::Event> events_write(1);
    std::vector<cl::Event> events_kernel(2);
    std::vector<cl::Event> events_read(1);

    q.enqueueMigrateMemObjects(ob_in, 0, nullptr, &events_write[0]); // 0 : migrate from host to dev

    // Launch kernel and compute kernel execution time
    q.enqueueTask(kernel_mm2s, &events_write, &events_kernel[0]);
    q.enqueueTask(kernel_s2mm, &events_write, &events_kernel[1]);

    void* dh;
    device.getInfo(CL_DEVICE_HANDLE, &dh);
    auto xrtHdl = xrtDeviceOpenFromXcl(dh);
    auto top = reinterpret_cast<const axlf*>(fileBuf.data());
    adf::registerXRT(xrtHdl, top->m_header.uuid);
    mygraph.run(1);
    mygraph.wait();
    q.finish();

    // Data transfer from device buffer to host buffer
    q.enqueueMigrateMemObjects(ob_out, 1, &events_kernel, &events_read[0]); // 1 : migrate from dev to host

    q.finish();
    std::cout << "INFO: Finish E2E execution" << std::endl;
    int err = 0;

    for (int i = 0; i < SAMPLES; i++) {
        if (device_output[i] != device_input[i]) {
            std::cout << "Error " << i << " " << device_input[i] << " " << device_output[i] << std::endl;
            err++;
        }
    }

    if (err) {
        std::cout << "Test failed" << std::endl;
    } else {
        std::cout << "Test passed" << std::endl;
    }

    return err;
}
