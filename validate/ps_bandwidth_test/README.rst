PS PL Bandwidth Test (C)
====================

This PS bandwidth test uses PS kernel to run PL bandwidth test.

**KEY CONCEPTS:** `burst read and write <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/AXI-Burst-Transfers>`__, Bandwidth

**EXCLUDED PLATFORMS:** 

 - Alveo U25 SmartNIC
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

   ./ps_bandwidth.exe -p $(GEN_DIR)

DETAILS
-------

This example contains bandwidth test kernels to validate FPGA DDR.

Extracting Memory Information and generate cfg file:
   
      - Platforminfo -j (path to xpfm) > platform_info.json
      - From the platform_info.json file we pick the memory info
      - Generate the meta data related to Memory Banks(DDR/HBM/HOST) to platform_bandwidth.cfg file


Using the ``sp`` option  in the platform_bandwidth.cfg file AXI-Master Port is connected to the IP:

::

   sp=bandwidth_1.input:MC_NOC0
   sp=bandwidth_1.output:MC_NOC0

Following is the real log reported when running the design on VCK5000:

:: 

   Throughput (Type: DDR) : 23461MB/s
   TEST PASSED

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.
