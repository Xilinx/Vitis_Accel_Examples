/**********
© Copyright 2020 Xilinx, Inc.
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
**********/
#include <adf.h>
#include "graph.h"
#include <unistd.h>
#include <fstream>
#include "xrt/xrt_kernel.h"
#include "xrt/xrt_aie.h"

int ITERATION=16320;

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

void ref_func(int32* din,int32 c[8],int32* dout,int size){
	static int32 d[8]={0,0,0,0,0,0,0,0};
	for(int i=0;i<size;i++){
		for(int j=0;j<7;j++){
			d[j]=d[j+1];
		}
		d[7]=*(din+i);
		int32 tmp=0;
		for(int j=0;j<8;j++){
			tmp+=d[j]*c[j];
		}
		*(dout+i)=tmp;
	}
}

int main(int argc, char ** argv) {
    char* xclbinFilename = argv[1];
    
    char* emu_mode = getenv("XCL_EMULATION_MODE");
    if (emu_mode != nullptr) {
        ITERATION = 4;
        printf("ITERATION: %d", ITERATION);
    }
    
    int BLOCK_SIZE_in_Bytes=ITERATION*1024;

	// Open xclbin
	auto device = xrt::device(0); //device index=0
	auto uuid = device.load_xclbin(xclbinFilename);

	auto din_buffer  = xrt::aie::bo (device, BLOCK_SIZE_in_Bytes, xrt::bo::flags::normal, /*memory group*/0);
    int* dinArray= din_buffer.map<int*>();
	auto dout_buffer  = xrt::aie::bo (device, BLOCK_SIZE_in_Bytes, xrt::bo::flags::normal, /*memory group*/0);
    int* doutArray= dout_buffer.map<int*>();
    
    int* doutRef=new int[BLOCK_SIZE_in_Bytes];

    int ret=0;
    int error=0;
    int32 coeff[8]={1,2,3,4,5,6,7,8};
    
    for(int i=0;i<ITERATION*1024/4;i++){
		dinArray[i]=i;
    }
    ref_func(dinArray,coeff,doutRef,ITERATION*1024/4);

	auto ghdl=xrt::graph(device,uuid,"gr");
	ghdl.run(ITERATION);

	din_buffer.async("gr.gmioIn",XCL_BO_SYNC_BO_GMIO_TO_AIE,BLOCK_SIZE_in_Bytes,/*offset*/0);
	
	auto dout_buffer_run=dout_buffer.async("gr.gmioOut",XCL_BO_SYNC_BO_AIE_TO_GMIO,BLOCK_SIZE_in_Bytes,/*offset*/0);

	ghdl.wait();
	std::cout<<"Waiting dout_buffer to complete transferring"<<std::endl;
	dout_buffer_run.wait();
	
    for(int i=0;i<ITERATION*1024/4;i++){
		if(doutArray[i]!=doutRef[i]){
			std::cout<<"ERROR:dout["<<i<<"]="<<doutArray[i]<<",gold="<<doutRef[i]<<std::endl;
			error++;
			break;
		}	
    }

    if(error==0){
	std::cout<<"TEST PASSED"<<std::endl;
    }else{
	std::cout<<"ERROR!"<<std::endl;
    }

	delete doutRef;
    return error;
};
