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
/*****************************************************************************************
*  GUI Flow :
*
*  By default this example supports 1DDR execution in GUI mode for
*  all the XSAs. To make this example to work with multi DDR XSAs
*  please follow steps mentioned below.
*
*  ***************************************************************************************
*  XSA  (2DDR):
*
*  1. Add a .cfg file in the <Project>/src folder with the following entries:
*  	    [Connecttivity]
*  	    sp=bandwidth_1.m_axi_gmem0:DDR[0]
*     	    sp=bandwidth_1.m_axi_gmem1:DDR[1]
*
*     For more number of DDR connections add more sp tags as shown above in the
*cfg file.
*     Note : Replace DDR[0] with HP0, DDR[1] with HP1 for embedded platforms(zc)
*  2.<Vitis Project> > Properties > C/C++ Build > Settings > Vitis V++ Kernel
*Linker
*                  > Miscellaneous > Other flags
*     --config ../src/<config_file>.cfg
*  3. Default number of banks for CLI flow is 2 banks, for GUI flow is 1 bank.
*     For 3 or 4 DDR connections, "#define NDDR_BANKS <3 or 4>" at the top of
*kernel.cpp
* *****************************************************************************************
*
*  CLI Flow:
*
*  In CLI flow makefile detects the DDR of a device on the basis of ddr_banks
*variable in config.mk file
*  and based on that automatically it adds all the flags that are necessary.
*This example can be
*  used similar to other examples in CLI flow, extra setup is not needed.
*
*********************************************************************************************/

#include "xcl2.hpp"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#ifndef NDDR_BANKS
#define NDDR_BANKS 1
#endif

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    std::string binaryFile = argv[1];

    cl_int err;
    cl::CommandQueue q;
    cl::Context context;
    cl::Kernel krnl_global_bandwidth;
    auto devices = xcl::get_xil_devices();
    // read_binary_file() is a utility API which will load the binaryFile
    // and will return the pointer to file buffer.
    auto fileBuf = xcl::read_binary_file(binaryFile);
    cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
    bool valid_device = false;
    for (unsigned int i = 0; i < devices.size(); i++) {
        auto device = devices[i];
        // Creating Context and Command Queue for selected Device
        OCL_CHECK(err, context = cl::Context(device, nullptr, nullptr, nullptr, &err));
        OCL_CHECK(err, q = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE, &err));

        std::cout << "Trying to program device[" << i << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
        cl::Program program(context, {device}, bins, nullptr, &err);
        if (err != CL_SUCCESS) {
            std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
        } else {
            std::cout << "Device[" << i << "]: program successful!\n";
            OCL_CHECK(err, krnl_global_bandwidth = cl::Kernel(program, "bandwidth", &err));
            valid_device = true;
            break; // we break because we found a valid device
        }
    }
    if (!valid_device) {
        std::cout << "Failed to program any device found, exit!\n";
        exit(EXIT_FAILURE);
    }

    size_t globalbuffersize = 1024 * 1024 * 256; /* 256 MB */

    /* Reducing the data size for emulation mode */
    char* xcl_mode = getenv("XCL_EMULATION_MODE");
    if (xcl_mode != nullptr) {
        globalbuffersize = 1024 * 1024; /* 1MB */
    }

    /* Input buffer */
    unsigned char* input_host = ((unsigned char*)malloc(globalbuffersize));
    if (input_host == nullptr) {
        printf(
            "Error: Failed to allocate host side copy of OpenCL source "
            "buffer of size %zu\n",
            globalbuffersize);
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < globalbuffersize; i++) {
        input_host[i] = i % 256;
    }

    short ddr_banks = NDDR_BANKS;

    /* Index for the ddr pointer array: 4=4, 3=3, 2=2, 1=2 */
    char num_buffers = ddr_banks;
    if (ddr_banks == 1) num_buffers = ddr_banks + (ddr_banks % 2);

    /* buffer[0] is input0
     * buffer[1] is output0
     * buffer[2] is input1
     * buffer[3] is output1 */
    cl::Buffer* buffer[num_buffers];

#if NDDR_BANKS > 1

    for (int i = 0; i < ddr_banks; i++) {
        buffer[i] = new cl::Buffer(context, CL_MEM_READ_WRITE, globalbuffersize, nullptr, &err);
        if (err != CL_SUCCESS) {
            printf("Error: Failed to allocate buffer in DDR bank %zu\n", globalbuffersize);
            return EXIT_FAILURE;
        }
    } /* End for (i < ddr_banks) */
#else
    OCL_CHECK(err, buffer[0] = new cl::Buffer(context, CL_MEM_READ_WRITE, globalbuffersize, nullptr, &err));
    OCL_CHECK(err, buffer[1] = new cl::Buffer(context, CL_MEM_READ_WRITE, globalbuffersize, nullptr, &err));
#endif

    /*
     * Using setArg(), i.e. setting kernel arguments, explicitly before copying
     * host
     * memory to device memory allowing runtime to associate buffer with correct
     * DDR banks automatically.
    */

    /* Set the kernel arguments */
    int arg_index = 0;
    int buffer_index = 0;
    cl_ulong num_blocks = globalbuffersize / 64;

    OCL_CHECK(err, err = krnl_global_bandwidth.setArg(arg_index++, *(buffer[buffer_index++])));
    OCL_CHECK(err, err = krnl_global_bandwidth.setArg(arg_index++, *(buffer[buffer_index++])));
#if NDDR_BANKS == 3
    OCL_CHECK(err, err = krnl_global_bandwidth.setArg(arg_index++, *(buffer[buffer_index++])));
#elif NDDR_BANKS > 3
    OCL_CHECK(err, err = krnl_global_bandwidth.setArg(arg_index++, *(buffer[buffer_index++])));
    OCL_CHECK(err, err = krnl_global_bandwidth.setArg(arg_index++, *(buffer[buffer_index++])));
#endif
    OCL_CHECK(err, err = krnl_global_bandwidth.setArg(arg_index++, num_blocks));

    double dbytes = globalbuffersize;
    double dmbytes = dbytes / (((double)1024) * ((double)1024));
    printf(
        "Starting kernel to read/write %.0lf MB bytes from/to global "
        "memory... \n",
        dmbytes);

    /* Write input buffer */
    /* Map input buffer for PCIe write */
    unsigned char* map_input_buffer0;
    OCL_CHECK(err,
              map_input_buffer0 = (unsigned char*)q.enqueueMapBuffer(
                  *(buffer[0]), CL_FALSE, CL_MAP_WRITE_INVALIDATE_REGION, 0, globalbuffersize, nullptr, nullptr, &err));
    OCL_CHECK(err, err = q.finish());

    /* prepare data to be written to the device */
    for (size_t i = 0; i < globalbuffersize; i++) {
        map_input_buffer0[i] = input_host[i];
    }
    OCL_CHECK(err, err = q.enqueueUnmapMemObject(*(buffer[0]), map_input_buffer0));

    OCL_CHECK(err, err = q.finish());

#if NDDR_BANKS > 3
    unsigned char* map_input_buffer1;
    OCL_CHECK(err,
              map_input_buffer1 = (unsigned char*)q.enqueueMapBuffer(
                  *(buffer[2]), CL_FALSE, CL_MAP_WRITE_INVALIDATE_REGION, 0, globalbuffersize, nullptr, nullptr, &err));
    OCL_CHECK(err, err = q.finish());

    /* Prepare data to be written to the device */
    for (size_t i = 0; i < globalbuffersize; i++) {
        map_input_buffer1[i] = input_host[i];
    }

    OCL_CHECK(err, err = q.enqueueUnmapMemObject(*(buffer[2]), map_input_buffer1));
    OCL_CHECK(err, err = q.finish());
#endif

    unsigned long start, end, nsduration;
    cl::Event event;

    /* Execute Kernel */
    OCL_CHECK(err, err = q.enqueueTask(krnl_global_bandwidth, nullptr, &event));
    OCL_CHECK(err, err = event.wait());
    end = OCL_CHECK(err, event.getProfilingInfo<CL_PROFILING_COMMAND_END>(&err));
    start = OCL_CHECK(err, event.getProfilingInfo<CL_PROFILING_COMMAND_START>(&err));
    nsduration = end - start;

    /* Copy results back from OpenCL buffer */
    unsigned char* map_output_buffer0;
    OCL_CHECK(err, map_output_buffer0 = (unsigned char*)q.enqueueMapBuffer(*(buffer[1]), CL_FALSE, CL_MAP_READ, 0,
                                                                           globalbuffersize, nullptr, nullptr, &err));
    OCL_CHECK(err, err = q.finish());

    std::cout << "Kernel Duration..." << nsduration << " ns" << std::endl;

    /* Check the results of output0 */
    for (size_t i = 0; i < globalbuffersize; i++) {
        if (map_output_buffer0[i] != input_host[i]) {
            printf("ERROR : kernel failed to copy entry %zu input %i output %i\n", i, input_host[i],
                   map_output_buffer0[i]);
            return EXIT_FAILURE;
        }
    }
#if NDDR_BANKS == 3
    unsigned char* map_output_buffer1;
    OCL_CHECK(err, map_output_buffer1 = (unsigned char*)q.enqueueMapBuffer(*(buffer[2]), CL_FALSE, CL_MAP_READ, 0,
                                                                           globalbuffersize, nullptr, nullptr, &err));
    OCL_CHECK(err, err = q.finish());

    /* Check the results of output1 */
    for (size_t i = 0; i < globalbuffersize; i++) {
        if (map_output_buffer1[i] != input_host[i]) {
            printf("ERROR : kernel failed to copy entry %zu input %i output %i\n", i, input_host[i],
                   map_output_buffer1[i]);
            return EXIT_FAILURE;
        }
    }
#endif

#if NDDR_BANKS > 3
    unsigned char* map_output_buffer1;
    OCL_CHECK(err, map_output_buffer1 = (unsigned char*)q.enqueueMapBuffer(*(buffer[3]), CL_FALSE, CL_MAP_READ, 0,
                                                                           globalbuffersize, nullptr, nullptr, &err));
    OCL_CHECK(err, err = q.finish());

    /* Check the results of output1 */
    for (size_t i = 0; i < globalbuffersize; i++) {
        if (map_output_buffer1[i] != input_host[i]) {
            printf("ERROR : kernel failed to copy entry %zu input %i output %i\n", i, input_host[i],
                   map_output_buffer1[i]);
            return EXIT_FAILURE;
        }
    }
#endif

#if NDDR_BANKS > 1
    for (int i = 0; i < ddr_banks; i++) {
        delete (buffer[i]);
    }
#else
    delete (buffer[0]);
    delete (buffer[1]);
#endif

    /* Profiling information */
    double dnsduration = ((double)nsduration);
    double dsduration = dnsduration / ((double)1000000000);

    double bpersec = (dbytes / dsduration);
    double gbpersec = bpersec / ((double)1024 * 1024 * 1024) * num_buffers;

    printf("Kernel completed read/write %.0lf MB bytes from/to global memory.\n", dmbytes);
    printf("Execution time = %f (sec) \n", dsduration);
    printf("Concurrent Read and Write Throughput = %f (GB/sec) \n", gbpersec);

    printf("TEST PASSED\n");
    return EXIT_SUCCESS;
}
