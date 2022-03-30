File Filter System Compiler
===========================

This is simple example showcasing the P2P and H2C file transfers in system compiler environment.

**KEY CONCEPTS:** `P2P <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/p2p>`__, SSD, Syncronization

**KEYWORDS:** vpp::p2p, vpp::h2c, file_buf, sync_output, custom_sync_outputs, sync_output_to_file

**EXCLUDED PLATFORMS:** 

 - All NoDMA Platforms, i.e u50 nodma etc
 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - All Versal Platforms, i.e vck190 etc
 - AWS VU9P F1
 - u2

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