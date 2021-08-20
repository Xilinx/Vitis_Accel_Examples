Validate Embedded Platform (C)
==============================

This example contains verify test and bandwidth test to validate embedded platforms.

Using the ``sp`` option  in the zcu102_base_dfx.cfg file AXI-Master Port is connected to the IP. 

::

   sp=bandwidth_1.input:HP0
   sp=bandwidth_1.output:HP0
   sp=bandwidth_2.input:HP1
   sp=bandwidth_2.output:HP1
   sp=bandwidth_3.input:HP2
   sp=bandwidth_3.output:HP2
   sp=bandwidth_4.input:HP3
   sp=bandwidth_4.output:HP3

Following is the real log reported while running the design on zcu102_base_dfx platform:

:: 

   INFO: Starting the Verify test....
   INFO: Found Platform
   INFO: Platform Name: Xilinx
   INFO: Platform Version: OpenCL 1.0
   INFO: Platform Profile: EMBEDDED_PROFILE
   INFO: Platform Extensions: cl_khr_icd
   INFO: INFO: Reading validate.xclbin
   INFO: Loading: 'validate.xclbin'
   INFO: Trying to program device edge
   INFO: Device program successful!
   INFO: Hello World
   INFO:
   INFO: Starting the Bandwidth test....
   INFO: Creating a kernel [bandwidth:{bandwidth_1}] for CU(1)
   INFO: Creating a kernel [bandwidth:{bandwidth_2}] for CU(2)
   INFO: Creating a kernel [bandwidth:{bandwidth_3}] for CU(3)
   INFO: Creating a kernel [bandwidth:{bandwidth_4}] for CU(4)
   INFO: Concurrent read and write throughput: 7318.5MB/s
   INFO: TEST PASSED
   INFO: INFO: host run completed.
