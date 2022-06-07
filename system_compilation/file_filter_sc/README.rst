File Filter System Compiler
===========================

This is simple example showcasing the P2P and H2C file transfers in system compiler environment.

**KEY CONCEPTS:** `P2P <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/p2p>`__, SSD, Syncronization

**KEYWORDS:** `vpp::p2p <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Special-Data-Transfer-Models>`__, `vpp::h2c <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Special-Data-Transfer-Models>`__, `file_buf <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Special-Data-Transfer-Models>`__, `sync_output <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/VPP_ACC-Class-API>`__, `custom_sync_outputs <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/VPP_ACC-Class-API>`__, `sync_output_to_file <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/VPP_ACC-Class-API>`__

.. raw:: html

 <details>

.. raw:: html

 <summary> 

 <b>EXCLUDED PLATFORMS:</b>

.. raw:: html

 </summary>
|
..

 - All NoDMA Platforms, i.e u50 nodma etc
 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - All Versal Platforms, i.e vck190 etc
 - Samsung U.2 SmartSSD
 - Alveo U25 SmartNIC
 - AWS VU9P F1

.. raw:: html

 </details>

.. raw:: html

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/filter.cpp
   src/filter.hpp
   src/main.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./host.exe

DETAILS
-------

This example demonstrates the P2P and H2C file transfers and protability across alveo with and without SSD. In this design we also capture user synchronization of input and output buffers to ACC.

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.