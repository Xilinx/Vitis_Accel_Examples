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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <iostream>
#include <vector>

#include "xcl2.hpp"

#include "bitmap.h"

void checkErrorStatus(cl_int error, const char* message)
{
  if (error != CL_SUCCESS)
  {
    printf("%s\n", message) ;
    exit(1) ;
  }
}

int main(int argc, char* argv[])
{
  if (argc < 3 || argc > 4 )
  {
    printf("Usage: %s <xclbin> <input bitmap> <golden bitmap(optional)>\n", argv[0]) ;
    return -1 ;
  }

  std::string binaryFile = argv[1];
  
  // Read the input bit map file into memory
  std::cout << "Reading input image...\n";
  const char* bitmapFilename = argv[2] ;
  const char* goldenFilename;
  int width = 128 ; // Default size
  int height = 128 ; // Default size

  BitmapInterface image(bitmapFilename) ;
  bool result = image.readBitmapFile() ;
  if (!result)
  {
    return EXIT_FAILURE ;
  }
  std::vector<int,aligned_allocator<int>> inputImage(image.numPixels() * sizeof(int));
  std::vector<int,aligned_allocator<int>> outImage(image.numPixels() * sizeof(int));
  if(inputImage.empty() || outImage.empty())
  {
    fprintf(stderr, "Unable to allocate the host memory!\n") ;
    return 0;
  }

  width = image.getWidth() ;
  height = image.getHeight() ;

  // Copy image host buffer
  memcpy(inputImage.data(), image.bitmap(), image.numPixels() * sizeof(int));

  // Set up OpenCL hardware and software constructs
  std::cout << "Setting up OpenCL hardware and software...\n";
  cl_int err = 0 ;
  unsigned fileBufSize;

  // The get_xil_devices will return vector of Xilinx Devices
  std::vector<cl::Device> devices = xcl::get_xil_devices();
  cl::Device device = devices[0];

  // Creating Context and Command Queue for selected Device
  cl::Context context(device);
  cl::CommandQueue q(context, device, CL_QUEUE_PROFILING_ENABLE);
  std::string device_name = device.getInfo<CL_DEVICE_NAME>();
  std::cout << "Found Device=" << device_name.c_str() << std::endl;

  // read_binary_file() command will find the OpenCL binary file created using the 
  // xocc compiler load into OpenCL Binary and return pointer to file buffer.
  char* fileBuf = xcl::read_binary_file(binaryFile, fileBufSize);
  cl::Program::Binaries bins{{fileBuf, fileBufSize}};
  devices.resize(1);
  cl::Program program(context, devices, bins);

  // These commands will allocate memory on the FPGA. The cl::Buffer objects can
  // be used to reference the memory locations on the device.
  cl::Buffer buffer_input(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, image.numPixels() * sizeof(int), inputImage.data());
  cl::Buffer buffer_output(context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, image.numPixels() * sizeof(int), outImage.data());

  cl::Event kernel_Event, read_Event, write_Event;

  // This call will extract a kernel out of the program we loaded in the previous line
  cl::Kernel krnl_medianFilter(program, "median");

  // set the kernel Arguments
  std::cout << "Setting arguments and enqueueing kernel...\n";
  krnl_medianFilter.setArg(0, buffer_input);
  krnl_medianFilter.setArg(1, buffer_output);
  krnl_medianFilter.setArg(2, width);
  krnl_medianFilter.setArg(3, height);

  // These commands will load the input_image and output_image vectors from the host
  // application into the buffer_input and buffer_output cl::Buffer objects.
  std::cout << "Writing input image to buffer...\n";
  err = q.enqueueMigrateMemObjects({buffer_input},0/* 0 means from host*/, NULL, &write_Event);

  checkErrorStatus(err, "Unable to enqueue write buffer") ;

  std::vector<cl::Event> eventList;
  eventList.push_back(write_Event);

  // This function will execute the kernel on the FPGA
  err = q.enqueueTask(krnl_medianFilter, &eventList, &kernel_Event);  
  checkErrorStatus(err, "Unable to enqueue Task") ;

  // Read back the image from the kernel
  std::cout << "Reading output image and writing to file...\n";
  eventList.clear();
  eventList.push_back(kernel_Event);
  err = q.enqueueMigrateMemObjects({buffer_output}, CL_MIGRATE_MEM_OBJECT_HOST, &eventList, &read_Event);
  checkErrorStatus(err, "Unable to enqueue read buffer") ;

  q.flush();
  q.finish();

  bool match = true;
  if (argc == 4){
      goldenFilename = argv[3];
      //Read the golden bit map file into memory
      BitmapInterface goldenImage(goldenFilename);
      result = goldenImage.readBitmapFile() ;
      if (!result)
      {
          std::cout << "ERROR:Unable to Read Golden Bitmap File "<< goldenFilename << std::endl;
          return EXIT_FAILURE ;
      }
      //Compare Golden Image with Output image
      if ( image.getHeight() != goldenImage.getHeight() || image.getWidth() != goldenImage.getWidth()){
          match = false;
      }else{
          int* goldImgPtr = goldenImage.bitmap();
          for (unsigned int i = 0 ; i < image.numPixels(); i++){
              if (outImage[i] != goldImgPtr[i]){
                  match = false;
                  printf ("Pixel %d Mismatch Output %x and Expected %x \n", i, outImage[i], goldImgPtr[i]);
                  break;
              }
          }
      }
  }

  delete[] fileBuf;

  // Write the final image to disk
  printf("Writing RAW Image \n");
  image.writeBitmapFile(outImage.data());

  std::cout << (match ? "TEST PASSED" : "TEST FAILED") << std::endl;
  return (match ? EXIT_SUCCESS : EXIT_FAILURE) ;

}
