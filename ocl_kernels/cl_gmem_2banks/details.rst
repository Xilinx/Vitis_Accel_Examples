Global Memory Two Banks (OpenCL Kernel)
=======================================

This example demonstrates how buffers can be created in two different
DDR(Global memory) banks.

By default, all the memory interfaces from all the kernels are connected
to a single global memory bank. As a result, only one memory interface
at a time can transfer data to and from the memory bank, limiting the
performance of the kernel. If the FPGA contains only one ``DDR`` (or
global) memory bank, this is the only option. However, some FPGA devices
contain multiple DDR memory banks. You can customize the connections
among the kernel memory interfaces and the DDR memory bank of such a
device by altering the default connection.

Banks need to be specified using ``sp`` tags in
apply_watermark.cfg file along with the port name and kernel name.

::

   [connectivity]
   sp=apply_watermark_1.input:DDR[0]
   sp=apply_watermark_1.output:DDR[1]
