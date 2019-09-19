Kernel Global Bandwidth Test
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

Design display following report in while running on board:

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
Concurrent Read and Write Throughput = 30721.837166 (MB/sec) 
TEST PASSED
```
