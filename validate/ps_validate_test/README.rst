PS Validate Kernel (C)
======================

This PS validate kernel tests PS kernel functionality.

**KEY CONCEPTS:** PS Kernel

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
 - Alveo U55
 - Alveo U50
 - Alveo U30
 - Alveo U200
 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - Versal VCK190
 - Alveo U250
 - AWS VU9P F1
 - Samsung SmartSSD Computation Storage Drive
 - Samsung U.2 SmartSSD
 - X3 Compute Shell
 - All NoDMA Platforms, i.e u50 nodma etc
 - Versal VCK5000

.. raw:: html

 </details>

.. raw:: html

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/bandwidth.cpp
   src/host.cpp
   src/ps_validate.c
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./ps_validate_test.exe -p $(GEN_DIR)

DETAILS
-------

This PS Validate Kernel test basic PS kernel functionality.

Following is the real log reported when running the design on VCK5000:

:: 

   TEST PASSED

To visit github.io of this repository, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.