/**
* Copyright (C) 2019-2022 Xilinx, Inc
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
#include <fstream>
#include <iostream>
#include <chrono>
#include "host.h"
#include "input.h"
#include "golden.h"

#define INPUT_SIZE 512
#define OUTPUT_SIZE 1024


int main(int argc, char* argv[]) {

    size_t input_size_in_bytes = INPUT_SIZE * sizeof(int);
    size_t output_size_in_bytes = OUTPUT_SIZE * sizeof(int);

    //TARGET_DEVICE macro needs to be passed from gcc command line
    if(argc != 2) {
		std::cout << "Usage: " << argv[0] <<" <xclbin>" << std::endl;
		return EXIT_FAILURE;
	}

    char* xclbinFilename = argv[1];
    
    // Creates a vector of DATA_SIZE elements with an initial value of 10 and 32
    // using customized allocator for getting buffer alignment to 4k boundary
    
    std::vector<cl::Device> devices;
    cl::Device device;
    std::vector<cl::Platform> platforms;
    bool found_device = false;

    //traversing all Platforms To find Xilinx Platform and targeted
    //Device in Xilinx Platform
    cl::Platform::get(&platforms);
    for(size_t i = 0; (i < platforms.size() ) & (found_device == false) ;i++){
        cl::Platform platform = platforms[i];
        std::string platformName = platform.getInfo<CL_PLATFORM_NAME>();
        if ( platformName == "Xilinx"){
            devices.clear();
            platform.getDevices(CL_DEVICE_TYPE_ACCELERATOR, &devices);
	    if (devices.size()){
		    device = devices[0];
		    found_device = true;
		    break;
	    }
        }
    }
    if (found_device == false){
       std::cout << "Error: Unable to find Target Device " 
           << device.getInfo<CL_DEVICE_NAME>() << std::endl;
       return EXIT_FAILURE; 
    }

    // Creating Context and Command Queue for selected device
    cl::Context context(device);
    cl::CommandQueue q1(context, device, CL_QUEUE_PROFILING_ENABLE);
    cl::CommandQueue q2(context, device, CL_QUEUE_PROFILING_ENABLE);

    // Load xclbin 
    std::cout << "Loading: '" << xclbinFilename << "'\n";
    std::ifstream bin_file(xclbinFilename, std::ifstream::binary);
    bin_file.seekg (0, bin_file.end);
    unsigned nb = bin_file.tellg();
    bin_file.seekg (0, bin_file.beg);
    char *buf = new char [nb];
    bin_file.read(buf, nb);
    
    // Creating Program from Binary File
    cl::Program::Binaries bins;
    bins.push_back({buf,nb});
    devices.resize(1);
    cl::Program program(context, devices, bins);
    
    // This call will get the kernel object from program. A kernel is an 
    // OpenCL function that is executed on the FPGA. 
    cl::Kernel krnl_mm2s(program,"mm2s");
    cl::Kernel krnl_s2mm(program,"s2mm");
    
    // These commands will allocate memory on the Device. The cl::Buffer objects can
    // be used to reference the memory locations on the device. 
    cl::Buffer buffer_in(context, CL_MEM_READ_ONLY, input_size_in_bytes);
    cl::Buffer buffer_out(context, CL_MEM_WRITE_ONLY, output_size_in_bytes);
    
    //set the kernel Arguments
    krnl_mm2s.setArg(0,buffer_in);
    krnl_mm2s.setArg(2,INPUT_SIZE);

    krnl_s2mm.setArg(0,buffer_out);
    krnl_s2mm.setArg(2,OUTPUT_SIZE);

    //We then need to map our OpenCL buffers to get the pointers
    short int *ptr_in = (short int *) q1.enqueueMapBuffer (buffer_in , CL_TRUE , CL_MAP_WRITE , 0, input_size_in_bytes);
    int *ptr_out = (int *) q2.enqueueMapBuffer (buffer_out , CL_TRUE , CL_MAP_READ , 0, output_size_in_bytes);

    //setting input data
    for(int i = 0 ; i< INPUT_SIZE*2; i++){
	    ptr_in[i] = cint16input[i];
    }

    // Data will be migrated to kernel space
    q1.enqueueMigrateMemObjects({buffer_in},0/* 0 means from host*/);

    auto start_time = std::chrono::high_resolution_clock::now();

    //Launch the Kernel
    q2.enqueueTask(krnl_s2mm);
    q1.enqueueTask(krnl_mm2s);

    // The result of the previous kernel execution will need to be retrieved in
    // order to view the results. This call will transfer the data from FPGA to
    // source_results vector
    q2.enqueueMigrateMemObjects({buffer_out},CL_MIGRATE_MEM_OBJECT_HOST);

    q1.finish();
    q2.finish();

    auto end_time = std::chrono::high_resolution_clock::now();
    auto elapsed_time = std::chrono::duration<double>(end_time - start_time).count();
    std::cout << "Runtime: " << elapsed_time << "s" << std::endl;

    //Verify the result
    int match = 0;
    for (int i = 0; i < OUTPUT_SIZE; i++) {
        if (ptr_out[i] != golden[i]) {
            printf("ERROR: Test failed! Error found in sample %d: golden: %d, hardware: %d\n", i, golden[i], ptr_out[i]);
            match = 1;
            break;
        }
    }

    q1.enqueueUnmapMemObject(buffer_in , ptr_in);
    q2.enqueueUnmapMemObject(buffer_out , ptr_out);

    q1.finish();
    q2.finish();

    std::cout << "TEST " << (match ? "FAILED" : "PASSED") << std::endl; 
    return (match ? EXIT_FAILURE :  EXIT_SUCCESS);

}
