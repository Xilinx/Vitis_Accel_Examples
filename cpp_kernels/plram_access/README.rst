PLRAM Memory Access (C)
=======================

This example shows the usage of PLRAM and how to use it with simple matrix multiplication (Row x Col).

**KEY CONCEPTS:** Vitis Memory Hierarchy, `PLRAMs <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/PLRAM-Configuration-and-Use>`__

**KEYWORDS:** `PLRAM <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/PLRAM-Configuration-and-Use>`__

.. raw:: html

 <details>

.. raw:: html

 <summary> 

 <b>EXCLUDED PLATFORMS:</b>

.. raw:: html

 </summary>
|
..

 - Alveo U25 SmartNIC
 - Alveo U30
 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - All Versal Platforms, i.e vck190 etc
 - Alveo U50
 - Alveo U55
 - Samsung SmartSSD Computation Storage Drive
 - Samsung U.2 SmartSSD
 - X3 Compute Shell
 - All NoDMA Platforms, i.e u50 nodma etc

.. raw:: html

 </details>

.. raw:: html

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/mmult.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./plram_access <mmult XCLBIN>

DETAILS
-------

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

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.