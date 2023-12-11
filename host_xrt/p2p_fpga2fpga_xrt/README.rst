P2P FPGA to FPGA Examples XRT (XRT Native API's)
================================================

This is simple example to explain P2P transfer between two FPGA devices using xrt native api's.

**KEY CONCEPTS:** `P2P <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Special-Data-Transfer-Models>`__, Multi-FPGA Execution, XDMA

**KEYWORDS:** xrt::bo::flags::p2p

.. raw:: html

 <details>

.. raw:: html

 <summary> 

 <b>EXCLUDED PLATFORMS:</b>

.. raw:: html

 </summary>
|
..

 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - Alveo U25 SmartNIC
 - Alveo U30
 - All Versal Platforms, i.e vck190 etc
 - AWS VU9P F1
 - All Platforms with 201910 Version
 - Samsung U.2 SmartSSD
 - Alveo U50 gen3x16 xdma 2019 Version

.. raw:: html

 </details>

.. raw:: html

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/increment.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./p2p_fpga2fpga_xrt -x <increment XCLBIN>

DETAILS
-------

This example illustrates the date transfer between two FPGA
using p2p without migrating the data via host memory space. In this example 
we create p2p buffer using ``p2p`` flag.

.. code:: cpp

   xrt::bo::flags flags = xrt::bo::flags::p2p;
   auto out1 = xrt::bo(device1, vector_size_bytes, flags, krnl.group_id(1));

Following is the real log reported while running on U50 platform and U200 platform:

::

   Open the device1 :0 
   Load the xclbin ./increment.xclbin
   Open the device2 : 1
   Allocate Buffer in Global Memory
   synchronize input buffer data to device global memory
   Execution of the kernel
   Get the output data from the device
   Total Time : 208670 (microseconds)
   TEST PASSED
