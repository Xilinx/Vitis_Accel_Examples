Multiple Cards System Compiler
==============================

This is simple example showcasing the P2P and H2C mode of application code controlling a multi-card accelerator design in system compiler environment.

**KEY CONCEPTS:** Multiple Cards, `P2P <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/p2p>`__, `H2C <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Special-Data-Transfer-Models>`__, Syncronization

**KEYWORDS:** `vpp::p2p <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Special-Data-Transfer-Models>`__, `vpp::h2c <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Special-Data-Transfer-Models>`__, vpp::squeue, `add_card <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/CU-Cluster-and-Multi-Card-Support>`__

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
 - Alveo U280

.. raw:: html

 </details>

.. raw:: html

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/main.cpp
   src/xacc.cpp
   src/xacc.hpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./host.exe 0 Makefile ./src/xacc.hpp ./src/xacc.cpp ./src/main.cpp

DETAILS
-------

This example demonstrates the P2P and H2C mode of application code controlling a multi-card accelerator design.

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.