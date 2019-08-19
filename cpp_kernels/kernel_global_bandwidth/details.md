Kernel Global Bandwidth Test
============================

This example tests the bandwidth of kernel interface with global memory for 4 cases using 1 DDR bank to 4 DDR banks.
If more than 1 input/output buffers are connected to one memory bank, these buffers can't be read concurrently to kernel's local memory. In such cases, buffers can be placed in different DDR banks to increase the bandwidth for concurrent transfers.

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
