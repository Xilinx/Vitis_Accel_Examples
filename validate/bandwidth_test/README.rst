Bandwidth Kernel (C)
====================

This Bandwidth Example test the maximum possible bandwidth between Kernel and Global Memory.

**KEY CONCEPTS:** `burst read and write <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/AXI-Burst-Transfers>`__, Bandwidth

**EXCLUDED PLATFORMS:** 

 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - Versal VCK190
 - All Platforms with 2019 Version
 - All Platforms with 2018 Version

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

   ./kernel_bw.exe -p $(GEN_DIR)

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

Following is the real log reported when running the design on U250:

:: 

   Found Platform
   Platform Name: Xilinx
   INFO: Reading ./test/bandwidth.xclbin
   Loading: './test/bandwidth.xclbin'
   Trying to program device xilinx_u250_gen3x16_xdma_3_202020_1
   Device program successful!
   Overall DDRs (Total 4) Throughput: 52207 MB/s
   TEST PASSED

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.