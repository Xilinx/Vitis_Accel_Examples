Multiple Compute Units (Asymmetrical) (C) 
==========================================

This is simple example of vector addition to demonstrate how to connect each compute unit to different banks and how to use these compute units in host applications

**KEY CONCEPTS:** `Multiple Compute Units <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Symmetrical-and-Asymmetrical-Compute-Units>`__, `Task Level Parallelism <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Task-Parallelism>`__

**EXCLUDED PLATFORMS:** 

 - Alveo U25 SmartNIC
 - Alveo U30
 - Alveo U50
 - Alveo U55
 - All Versal Platforms, i.e vck190 etc
 - Samsung SmartSSD Computation Storage Drive
 - Samsung U.2 SmartSSD
 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - X3 Compute Shell
 - All NoDMA Platforms, i.e u50 nodma etc

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/vadd.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./multiple_cus_asymmetrical <vadd XCLBIN>

DETAILS
-------

This example demonstrates how multiple compute units can do parallel
processing where the compute units are asymmetrical i.e. connected to
different memory banks.

Kernel can be connected to different banks using vadd.cfg file as below:

::

   [connectivity]
   nk=vadd:4:vadd_1.vadd_2.vadd_3.vadd_4
   sp=vadd_1.in1:DDR[0]
   sp=vadd_1.in2:DDR[0]
   sp=vadd_1.out:DDR[0]
   sp=vadd_2.in1:DDR[1]
   sp=vadd_2.in2:DDR[1]
   sp=vadd_2.out:DDR[1]
   sp=vadd_3.in1:PLRAM[0]
   sp=vadd_3.in2:PLRAM[0]
   sp=vadd_3.out:PLRAM[0]
   sp=vadd_4.in1:PLRAM[1]
   sp=vadd_4.in2:PLRAM[1]
   sp=vadd_4.out:PLRAM[1]

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

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.