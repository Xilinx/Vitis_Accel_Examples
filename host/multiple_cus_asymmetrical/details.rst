Multiple Compute Units (Asymmetrical) (C)
=========================================

This example demonstrates how multiple compute units can do parallel
processing where the compute units are asymmetrical i.e. connected to
different memory banks.

Kernel can be connected to different banks using vadd.cfg file as below:

::

   [connectivity]
   nk=vadd:4:vadd_1.vadd_2.vadd_3.vadd_4
   sp=vadd_1.in1:DDR[0]
   sp=vadd_1.in2:DDR[0]
   sp=vadd_1.out_r:DDR[0]
   sp=vadd_2.in1:DDR[1]
   sp=vadd_2.in2:DDR[1]
   sp=vadd_2.out_r:DDR[1]
   sp=vadd_3.in1:PLRAM[0]
   sp=vadd_3.in2:PLRAM[0]
   sp=vadd_3.out_r:PLRAM[0]
   sp=vadd_4.in1:PLRAM[1]
   sp=vadd_4.in2:PLRAM[1]
   sp=vadd_4.out_r:PLRAM[1]

Some of the vadd compute units are connected to DDR banks and some are
connected to PLRAMs. ``nk`` option can be used to specify how many
compute unit are needed for ``vadd`` kernel. It also allows to give
names of user choice. ``sp`` option can be used to provide connection of
compute units to target memory (DDR, PLRAM).

``vadd.cfg`` file should be included in v++ command line as below:

::

   --config vadd.cfg

As each compute unit are connected to diferrent DDR/PLRAM, so host
program has to create kernel object specific to compute unit as below:

.. code:: cpp

   cl::Kernel cu0_krnls = cl::Kernel(program, "vadd:{vadd_1}");

The kernel object which is created above is very specific to ``vadd_1``
compute unit. Using this Kernel Object, host can directly access to this
fix compute unit.
