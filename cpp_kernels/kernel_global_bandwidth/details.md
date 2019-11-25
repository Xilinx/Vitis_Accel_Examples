Kernel Global Bandwidth
============================

This example tests global memory access bandwidth of kernel. Kernel increase the bandwidth by accessing multiple DDR banks through different interfaces. 

User can customize these connections to global memory by explicitly specifying the memory banks for different inputs/outputs.
Seperate bundle names need to be specified for each buffer to be placed in different bank.

```c++
#pragma HLS INTERFACE m_axi port = input offset = slave bundle = gmem0
#pragma HLS INTERFACE m_axi port = output offset = slave bundle = gmem1
```
Banks need to be specified using the `--sp` tags in the kernel linking stage along with the `m_axi` interface name and kernel name.
```
--sp apply_watermark_1.m_axi_gmem0:DDR[0] --sp apply_watermark_1.m_axi_gmem1:DDR[1]
```

Following is the log reported while running the design on U200 platform :

```
./kernel_global ./build_dir.hw.xilinx_u200_xdma_201830_2/krnl_kernel_global.xclbin
Found Platform
Platform Name: Xilinx
Found Device=xilinx_u200_xdma_201830_2
INFO: Reading ./build_dir.hw.xilinx_u200_xdma_201830_2/krnl_kernel_global.xclbin
Loading: './build_dir.hw.xilinx_u200_xdma_201830_2/krnl_kernel_global.xclbin'
Starting kernel to read/write 1024 MB bytes from/to global memory... 
Kernel Duration...66662680 ns
Kernel completed read/write 1024 MB bytes from/to global memory.
Execution time = 0.066663 (sec) 
Concurrent Read and Write Throughput = 30.72 (GB/sec) 
TEST PASSED
```
  GUI Flow :
      
  By default this example supports 1DDR execution in GUI mode for 
  all the XSAs. To make this example to work with multi DDR XSAs
  please follow steps mentioned below.
  XSA  (2DDR):
              
  1. Add a .ini file in the <Project>/src folder with the following entries:
  	    [Connecttivity]
  	    sp=bandwidth_1.m_axi_gmem0:DDR[0]
     	    sp=bandwidth_1.m_axi_gmem1:DDR[1]         
  2.<Vitis Project> > Properties > C/C++ Build > Settings > Vitis V++ Kernel Compiler 
                  > Miscellaneous > Other flags
  3.In "Other flags" box enter following
     a. --max_memory_ports all 
  4.<Vitis Project> > Properties > C/C++ Build > Settings > Vitis V++ Kernel Linker
                  > Miscellaneous > Other flags
     --config ../src/<config_file.ini>

  XSA  (3DDR):
             
  1. Add a .ini file in the <Project>/src folder with the following entries:
  	    [Connecttivity]
  	    sp=bandwidth_1.m_axi_gmem0:DDR[0]
     	    sp=bandwidth_1.m_axi_gmem1:DDR[1]         
     	    sp=bandwidth_1.m_axi_gmem2:DDR[2]         
  2.<Vitis Project> > Properties > C/C++ Build > Settings > Vitis V++ Kernel Compiler 
                  > Miscellaneous > Other flags
  3.In "Other flags" box enter following
     a. --max_memory_ports all 
  4.<Vitis Project> > Properties > C/C++ Build > Settings > Vitis V++ Kernel Linker
                  > Miscellaneous > Other flags
     --config ../src/<config_file.ini>
  5.Define NDDR_BANKS 3 in kernel "#define NDDR_BANKS 3" at the top of kernel.cl 
 
  XSA  (4DDR):
      
  Note: The selected platform must support 4DDR.             
              
  1. Add a .ini file in the <Project>/src folder with the following entries:
  	    [Connecttivity]
  	    sp=bandwidth_1.m_axi_gmem0:DDR[0]
     	    sp=bandwidth_1.m_axi_gmem1:DDR[1]         
     	    sp=bandwidth_1.m_axi_gmem2:DDR[2]         
     	    sp=bandwidth_1.m_axi_gmem2:DDR[3]         
  2.<Vitis Project> > Properties > C/C++ Build > Settings > Vitis V++ Kernel Compiler 
                  > Miscellaneous > Other flags
  3.In "Other flags" box enter following
     a. --max_memory_ports all 
  4.<Vitis Project> > Properties > C/C++ Build > Settings > Vitis V++ Kernel Linker
                  > Miscellaneous > Other flags
     --config ../src/<config_file.ini>
  5.Define NDDR_BANKS 4 in kernel "#define NDDR_BANKS 4" at the top of kernel.cl 
 

