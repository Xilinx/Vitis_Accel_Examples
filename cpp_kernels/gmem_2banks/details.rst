Global Memory Two Banks
=======================

This example demonstrates how buffers can be created in two different
DDR(Global memory) banks.

By default, all the memory interfaces from all the kernels are connected
to a single global memory bank. As a result, only one memory interface
at a time can transfer data to and from the memory bank, limiting the
performance of the kernel. If the FPGA contains only one DDR (or global)
memory bank, this is the only option. However, some FPGA devices contain
multiple DDR memory banks which allows us to customize the connections
among the kernel memory interfaces and the DDR memory bank of a device
by altering the default connection.

Seperate bundle names need to be specified for each buffer to be placed
in different bank.

.. code:: cpp

   #pragma HLS INTERFACE m_axi port = input offset = slave bundle = gmem0
   #pragma HLS INTERFACE m_axi port = output offset = slave bundle = gmem1

This example uses watermark application to demonstrate the utilization
of multiple DDRs for full bandwidth. Input image is placed into
``Bank0`` and kernel will read the input Image and write the output
image to ``Bank1``. Banks need to be specified using ``sp`` tags in
apply_watermark.ini file along with the port name and kernel name.

::

   [connectivity]
   sp=apply_watermark_1.input:DDR[0]
   sp=apply_watermark_1.output:DDR[1]
