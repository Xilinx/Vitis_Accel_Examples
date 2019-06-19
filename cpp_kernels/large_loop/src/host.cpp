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

/*******************************************************************************
Description: 

    This is a CNN (Convolutional Neural Network) convolutional layer based
    example to showcase the effectiveness of using multiple compute units when
    the base algorithm consists of multiple nested loops with large loop count.    

*******************************************************************************/

#include <cassert>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <unistd.h>

//OpenCL utility layer include
#include "defns.h"
#include "xcl2.hpp"

#define WORK_GROUP 4
#define WORK_ITEM_PER_GROUP 1

#define DATA_SIZE OChan *OSize *OSize

// Software solution
void convGolden(int *weight, int *image, int *out, int i_chan, int o_chan) {
    // Runs over output filters
    for (int output = 0; output < o_chan; output++) {
        // Runs over output pixel in Y-direction
        for (int y = 0; y < OSize; y++) {
            // Runs over output pixel in X-direction
            for (int x = 0; x < OSize; x++) {
                short acc = 0;
                // Runs over each input channel of input feature map
                for (int input = 0; input < i_chan; input++) {
                    // Runs over filter window
                    for (int i = 0; i < WSize; i++) {
                        // Runs over filter windows
                        for (int j = 0; j < WSize; j++) {

                            // Calculate input padding boundaries
                            int xVal = x * Stride + j - Padding,
                                yVal = y * Stride + i - Padding;

                            // Convolution operation
                            if (yVal >= 0 && yVal < ISize && xVal >= 0 &&
                                xVal < ISize) {
                                acc +=
                                    (short)
                                        image[(input * ISize + yVal) * ISize +
                                              xVal] *
                                    (short)weight[((output * WInChan + input) *
                                                       WSize +
                                                   i) *
                                                      WSize +
                                                  j];
                            }
                        }
                        // Update each output pixel / output filter
                        out[(output * OSize + y) * OSize + x] = acc;
                    }
                }
            }
        }
    }
}

uint64_t run_opencl_cnn(std::vector<cl::Device> &devices,
                        cl::CommandQueue &q,
                        cl::Context &context,
                        std::string &device_name,
                        std::string &binaryFile,
                        bool good,
                        int size,
                        std::vector<int, aligned_allocator<int>> &weight,
                        std::vector<int, aligned_allocator<int>> &image,
                        std::vector<int, aligned_allocator<int>> &output,
                        int i_chan,
                        int o_chan) {

    cl_int err;
    unsigned fileBufSize;

    auto fileBuf = xcl::read_binary_file(binaryFile, fileBufSize);
    cl::Program::Binaries bins{{fileBuf, fileBufSize}};
    devices.resize(1);
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
    cl::Kernel krnl_cnn_conv;

    if (good) {
        OCL_CHECK(err, krnl_cnn_conv = cl::Kernel(program, "cnn_GOOD", &err));
    } else {
        OCL_CHECK(err, krnl_cnn_conv = cl::Kernel(program, "cnn_BAD", &err));
    }

    std::cout << "Starting " << (good ? "GOOD" : "BAD") << " Kernel"
              << std::endl;

    size_t image_size_bytes = sizeof(int) * i_chan * ISize * ISize;
    size_t weight_size_bytes = sizeof(int) * o_chan * WInChan * WSize * WSize;
    size_t output_size_bytes = sizeof(int) * o_chan * OSize * OSize;

    // Allocate Buffer in Global Memory
    OCL_CHECK(err,
              cl::Buffer buffer_image(context,
                                      CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                      image_size_bytes,
                                      image.data(),
                                      &err));
    OCL_CHECK(err,
              cl::Buffer buffer_weight(context,
                                       CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                                       weight_size_bytes,
                                       weight.data(),
                                       &err));
    OCL_CHECK(err,
              cl::Buffer buffer_output(context,
                                       CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                                       output_size_bytes,
                                       output.data(),
                                       &err));

    //Set the Kernel Arguments
    int narg = 0;
    OCL_CHECK(err, err = krnl_cnn_conv.setArg(narg++, buffer_image));
    OCL_CHECK(err, err = krnl_cnn_conv.setArg(narg++, buffer_weight));
    OCL_CHECK(err, err = krnl_cnn_conv.setArg(narg++, buffer_output));
    OCL_CHECK(err, err = krnl_cnn_conv.setArg(narg++, size));
    OCL_CHECK(err, err = krnl_cnn_conv.setArg(narg++, i_chan));
    OCL_CHECK(err, err = krnl_cnn_conv.setArg(narg++, o_chan));

    OCL_CHECK(err,
              err = q.enqueueMigrateMemObjects({buffer_image, buffer_weight},
                                               0 /* 0 means from host*/));

    std::cout << "Begin " << (good ? "GOOD" : "BAD") << " Kernel" << std::endl;

    uint64_t duration = 0;

    if (good) {
        int work_group = WORK_GROUP;

        cl::Event events[work_group];

        auto kernel_start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < work_group; i++) {
            OCL_CHECK(err, err = krnl_cnn_conv.setArg(narg + 0, i));
            OCL_CHECK(err, err = krnl_cnn_conv.setArg(narg + 1, work_group));

            OCL_CHECK(err,
                      err = q.enqueueTask(krnl_cnn_conv, NULL, &events[i]));
        }
        OCL_CHECK(err, err = q.finish());
        auto kernel_end = std::chrono::high_resolution_clock::now();
        auto kernel_time = std::chrono::duration<uint64_t, std::nano>(
            kernel_end - kernel_start);
        duration = kernel_time.count();
    } else {
        cl::Event event;

        auto kernel_start = std::chrono::high_resolution_clock::now();
        OCL_CHECK(err, err = q.enqueueTask(krnl_cnn_conv, NULL, &event));
        OCL_CHECK(err, err = q.finish());
        auto kernel_end = std::chrono::high_resolution_clock::now();
        auto kernel_time = std::chrono::duration<uint64_t, std::nano>(
            kernel_end - kernel_start);
        duration = kernel_time.count();
    }

    //Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err,
              err = q.enqueueMigrateMemObjects({buffer_output},
                                               CL_MIGRATE_MEM_OBJECT_HOST));
    OCL_CHECK(err, err = q.finish());
    delete[] fileBuf;

    std::cout << "Finished " << (good ? "GOOD" : "BAD") << " Kernel"
              << std::endl;
    return duration;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <GOOD XCLBIN File>"
                  << " <BAD XCLBIN File>" << std::endl;
        return EXIT_FAILURE;
    }

    int i_chan = IChan;
    int o_chan = OChan;

    int size = DATA_SIZE;
    cl_int err;

    const char *xcl_emu = getenv("XCL_EMULATION_MODE");
    if (xcl_emu && !strcmp(xcl_emu, "hw_emu")) {
        i_chan = 1;
        o_chan = 1;

        size = o_chan * OSize * OSize;

        printf("\nOriginal Dataset is Reduced for Faster Execution of Hardware "
               "Emulation Flow\n");
        printf("\t#Input_Channels (IChan)            = %d (Original : 96 )\n",
               i_chan);
        printf("\t#Weight_Output_Channels (WOutChan) = %d (Original : 256)\n\n",
               o_chan);
    }

    // Allocate Memory in Host (Image, Weights and Output)
    size_t image_size_bytes = sizeof(int) * i_chan * ISize * ISize;
    size_t weight_size_bytes = sizeof(int) * o_chan * WInChan * WSize * WSize;
    size_t output_size_bytes = sizeof(int) * o_chan * OSize * OSize;

    std::vector<int, aligned_allocator<int>> image(image_size_bytes);
    std::vector<int, aligned_allocator<int>> weight(weight_size_bytes);
    std::vector<int, aligned_allocator<int>> source_good_hw_results(
        output_size_bytes);
    std::vector<int, aligned_allocator<int>> source_bad_hw_results(
        output_size_bytes);
    std::vector<int, aligned_allocator<int>> source_sw_results(
        output_size_bytes);

    // Initialize Image, Weights & Output Host Buffers
    for (int i = 0; i < i_chan * ISize * ISize; i++)
        image[i] = i % 255;

    for (int i = 0; i < o_chan * WInChan * WSize * WSize; i++)
        weight[i] = i % 255;

    for (int i = 0; i < o_chan * OSize * OSize; i++)
        source_sw_results[i] = source_good_hw_results[i] =
            source_bad_hw_results[i] = 0;

    convGolden(
        weight.data(), image.data(), source_sw_results.data(), i_chan, o_chan);

    //OPENCL HOST CODE AREA START
    //Create Program and Kernels
    auto devices = xcl::get_xil_devices();
    auto device = devices[0];

    OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(err,
              cl::CommandQueue q(context,
                                 device,
                                 CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE |
                                     CL_QUEUE_PROFILING_ENABLE,
                                 &err));
    std::string device_name = device.getInfo<CL_DEVICE_NAME>();
    std::string binaryFile;

    binaryFile = argv[2];

    uint64_t bad_duration = run_opencl_cnn(devices,
                                           q,
                                           context,
                                           device_name,
                                           binaryFile,
                                           false,
                                           size,
                                           weight,
                                           image,
                                           source_bad_hw_results,
                                           i_chan,
                                           o_chan);

    binaryFile = argv[1];

    uint64_t good_duration = run_opencl_cnn(devices,
                                            q,
                                            context,
                                            device_name,
                                            binaryFile,
                                            true,
                                            size,
                                            weight,
                                            image,
                                            source_good_hw_results,
                                            i_chan,
                                            o_chan);
    //OPENCL HOST CODE AREA END

    // Compare the results of the Device to the simulation
    bool match = true;
    for (int i = 0; i < size; i++) {
        /* if bad_duration is 0 then the kernel was unable to be produced for
         * the hardware thus there's no reason to check the results */
        if (bad_duration != 0) {
            if (source_bad_hw_results[i] != source_sw_results[i]) {
                std::cout << "Error: Result mismatch in bad kernel"
                          << std::endl;
                std::cout << "i = " << i
                          << " CPU result = " << source_sw_results[i]
                          << " Device result = " << source_bad_hw_results[i]
                          << std::endl;
                match = false;
                break;
            }
        }
        if (source_good_hw_results[i] != source_sw_results[i]) {
            std::cout << "Error: Result mismatch in good kernel" << std::endl;
            std::cout << "i = " << i << " CPU result = " << source_sw_results[i]
                      << " Device result = " << source_good_hw_results[i]
                      << std::endl;
            match = false;
            break;
        }
    }

    if (bad_duration != 0) {
        std::cout << "BAD duration = " << bad_duration << " ns" << std::endl;
    }
    std::cout << "GOOD duration = " << good_duration << " ns" << std::endl;

    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;
    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
