PLRAM Memory Access
===================

This example demonstrates how ``PLRAM`` feature of the Vitis memory
subsystem and how they integrate with the Vitis design process.

PLRAM is small shared memory which is built using the on-chip memory
resources of the FPGA fabric. It is intended to provide a small amount
of data storage that application kernels can share and access
rapidly(lowest latency). PLRAM behaves just like the DDR memory
resources managed by the Vitis memory subsystem.

PLRAM can be assigned to a buffer by using ``sp`` tags in the mmult.cfg file along with the port name and kernel name.

::

   [connectivity]
   sp=mmult_1.a:PLRAM[0]
   sp=mmult_1.b:DDR[0]
   sp=mmult_1.c:PLRAM[1]
