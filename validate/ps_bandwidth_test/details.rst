PS PL Bandwidth Test (C)
===================

This PS bandwidth test uses PS kernel to run PL bandwidth test.

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
