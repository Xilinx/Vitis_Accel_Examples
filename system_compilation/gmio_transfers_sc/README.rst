GMIO Transfers System Compiler
==============================

This is gmio transfers example which explains different ways of data movements between host and device for compute unit processing.

**KEY CONCEPTS:** `System Compiler <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Using-Vitis-System-Compilation-Mode>`__

**KEYWORDS:** `DATA_COPY <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Guidance-Macros>`__, `ZERO_COPY <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Guidance-Macros>`__, `ACCESS_PATTERN <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Guidance-Macros>`__, `SEQUENTIAL <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Quick-Start-Example>`__, `RANDOM <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Guidance-Macros>`__

.. raw:: html

 <details>

.. raw:: html

 <summary> 

 <b>EXCLUDED PLATFORMS:</b>

.. raw:: html

 </summary>
|
..

 - Samsung U.2 SmartSSD
 - All NoDMA Platforms, i.e u50 nodma etc
 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - All Versal Platforms, i.e vck190 etc
 - Alveo U280
 - AWS VU9P F1

.. raw:: html

 </details>

.. raw:: html

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/acc.cpp
   src/acc.hpp
   src/main.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./host.exe

DETAILS
-------

This example demonstrates the Sequential & Randon data transfer between host and device compute units.

- ACCESS_PATTERN(<port>, <pattern>)
This macro is used to specify the data access pattern on a given CU port/memory interface. Based on the access pattern system compiler can generate an appropriate data mover plugin for the kernel. Where <port> is the kernel port and  <pattern> can be sequential/random.

- DATA_COPY(<port>, <port>[<num>])
Specifies the amount of data to be copied by the data mover at runtime between host and device for CU processing for given port. The "port" needs to be replaced by port name ( kernel function argument name). The "num" specifies the number of data elements to be transferred the size of data. The num can be a constant or it can be itself an expression in terms of scalar kernel arguments.

- ZERO_COPY(<port>, <port>[<num>])
Do not use a data mover plugin. The CU is assumed to be in direct connection with global DDR memory. It lets the kernel use a M-AXI interface to directly talk to the device DDR. The CU is responsible for any on-chip caching of data needed.  The "port" needs to be replaced by port name ( CU argument name). The "num" specifies the number of data elements to be transferred essentially the size of data. The "num" can be a constant or it can be itself an expression in terms of some scalar CU arguments.

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.