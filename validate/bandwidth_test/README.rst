Bandwidth Kernel (C)
====================

This Bandwidth Example test the maximum possible bandwidth between Kernel and Global Memory.

**KEY CONCEPTS:** Burst Read/Write, Bandwidth

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   zc
   vck190
   2019
   2018

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/bandwidth.cpp
   src/host.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./kernel_bw.exe platform_test_path

DETAILS
-------

This example contains bandwidth test kernels to validate FPGA DDR.

Extracting Memory Information and generate cfg file:
   
      - Platforminfo -j (path to xpfm) > platform_info.json
      - From the platform_info.json file we pick the memory info
      - Generate the meta data related to Memory Banks(DDR/HBM/HOST) to platform_bandwidth.cfg file


Using the ``sp`` option  in the platform_bandwidth.cfg file AXI-Master Port is connected to the IP:

::

   sp=bandwidth_1.input:DDR[0]
   sp=bandwidth_1.output:DDR[0]
   sp=bandwidth_2.input:DDR[1]
   sp=bandwidth_2.output:DDR[1]
   sp=bandwidth_3.input:DDR[2]
   sp=bandwidth_3.output:DDR[2]
   sp=bandwidth_4.input:DDR[3]
   sp=bandwidth_4.output:DDR[3]

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.