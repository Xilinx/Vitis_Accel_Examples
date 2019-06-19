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
#include "bitmap.h"
#include "xcl2.hpp"
#include <vector>

int main(int argc, char *argv[]) {
    cl_int err;
    unsigned fileBufSize;
    if (argc < 3 || argc > 4) {
        std::cout << "Usage: " << argv[0] << " <XCLBIN File>"
                  << " <input bitmap> <golden bitmap(optional)>" << std::endl;
        return EXIT_FAILURE;
    }
    std::string binaryFile = argv[1];
    std::string bitmapFilename = argv[2];

    //Read the input bit map file into memory
    BitmapInterface image(bitmapFilename.data());
    bool result = image.readBitmapFile();
    if (!result) {
        std::cout << "ERROR:Unable to Read Input Bitmap File "
                  << bitmapFilename.data() << std::endl;
        return EXIT_FAILURE;
    }
    auto width = image.getWidth();
    auto height = image.getHeight();

    //Allocate Memory in Host Memory
    auto image_size = image.numPixels();
    size_t image_size_bytes = image_size * sizeof(int);
    std::vector<int, aligned_allocator<int>> inputImage(image_size);
    std::vector<int, aligned_allocator<int>> outImage(image_size);

    // Copy image host buffer
    memcpy(inputImage.data(), image.bitmap(), image_size_bytes);

    //OPENCL HOST CODE AREA START

    auto devices = xcl::get_xil_devices();
    auto device = devices[0];

    OCL_CHECK(err, cl::Context context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(
        err,
        cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE, &err));
    OCL_CHECK(err,
              std::string device_name = device.getInfo<CL_DEVICE_NAME>(&err));

    auto fileBuf = xcl::read_binary_file(binaryFile, fileBufSize);
    cl::Program::Binaries bins{{fileBuf, fileBufSize}};
    devices.resize(1);
    OCL_CHECK(err, cl::Program program(context, devices, bins, NULL, &err));
    OCL_CHECK(err,
              cl::Kernel krnl_applyWatermark(program, "apply_watermark", &err));

    OCL_CHECK(err,
              cl::Buffer buffer_inImage(context,
                                        CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR,
                                        image_size_bytes,
                                        inputImage.data(),
                                        &err));
    OCL_CHECK(
        err,
        cl::Buffer buffer_outImage(context,
                                   CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR,
                                   image_size_bytes,
                                   outImage.data(),
                                   &err));

    /* 
     * Using setArg(), i.e. setting kernel arguments, explicitly before enqueueMigrateMemObjects(), 
     * i.e. copying host memory to device memory,  allowing runtime to associate buffer with correct
     * DDR banks automatically. 
    */

    krnl_applyWatermark.setArg(0, buffer_inImage);
    krnl_applyWatermark.setArg(1, buffer_outImage);
    krnl_applyWatermark.setArg(2, width);
    krnl_applyWatermark.setArg(3, height);

    //Copy input Image to device global memory
    OCL_CHECK(err,
              err = q.enqueueMigrateMemObjects({buffer_inImage},
                                               0 /* 0 means from host*/));

    //Launch the Kernel
    OCL_CHECK(err, err = q.enqueueTask(krnl_applyWatermark));

    //Copy Result from Device Global Memory to Host Local Memory
    OCL_CHECK(err,
              err = q.enqueueMigrateMemObjects({buffer_outImage},
                                               CL_MIGRATE_MEM_OBJECT_HOST));
    OCL_CHECK(err, err = q.finish());
    //OPENCL HOST CODE AREA END

    //Compare Golden Image with Output image
    bool match = 1;
    if (argc == 4) {
        std::string goldenFilename = argv[3];
        //Read the golden bit map file into memory
        BitmapInterface goldenImage(goldenFilename.data());
        result = goldenImage.readBitmapFile();
        if (!result) {
            std::cout << "ERROR:Unable to Read Golden Bitmap File "
                      << goldenFilename.data() << std::endl;
            return EXIT_FAILURE;
        }
        if (image.getHeight() != goldenImage.getHeight() ||
            image.getWidth() != goldenImage.getWidth()) {
            match = 0;
        } else {
            int *goldImgPtr = goldenImage.bitmap();
            for (unsigned int i = 0; i < image.numPixels(); i++) {
                if (outImage[i] != goldImgPtr[i]) {
                    match = 0;
                    printf("Pixel %d Mismatch Output %x and Expected %x \n",
                           i,
                           outImage[i],
                           goldImgPtr[i]);
                    break;
                }
            }
        }
    }
    // Write the final image to disk
    image.writeBitmapFile(outImage.data());

    delete[] fileBuf;

    std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;
    return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
