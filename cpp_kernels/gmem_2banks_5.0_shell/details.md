Global Memory Two Banks for 5.0 shell
======================================

This example demonstrates how buffers can be created in two different DDR(Global memory) banks.

By default, all the memory interfaces from all the kernels are connected to a single global memory bank. As a result, only one memory interface at a time can transfer data to and from the memory bank, limiting the performance of the kernel. If the FPGA contains only one DDR (or global) memory bank, this is the only option. However, some FPGA devices contain multiple DDR memory banks. You can customize the connections among the kernel memory interfaces and the DDR memory bank of such a device by altering the default connection.

Seperate bundle names need to be specified for each buffer to be placed in different bank.

```c++
#pragma HLS INTERFACE m_axi port = input offset = slave bundle = gmem0
#pragma HLS INTERFACE m_axi port = output offset = slave bundle = gmem1
```
Banks need to be specified using the `--sp` tags in the kernel linking stage along with the `m_axi` interface name and kernel name.
```
--sp apply_watermark_1.m_axi_gmem0:bank0 --sp apply_watermark_1.m_axi_gmem1:bank1
```
