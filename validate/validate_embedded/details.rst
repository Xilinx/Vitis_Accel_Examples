Validate Zynq UltraScale Platform (C)
=====================================

This example contains verify test and bandwidth test to validate EDGE platforms.

In the verify test we have extremely simple HLS C Kernel to verify that the platform has basic functionality. It will make a call to the kernel with an empty global buffer. The kernel will then write the string of characters "Hello World" into the buffer and return. The host will copy this buffer locally and then print out the resulting buffer contents.

In the bandwidth test we try to get the maximum possible bandwidth between Kernel and Global Memory.

Using the ``sp`` option  in the platform_bandwidth.cfg file AXI-Master Port is connected to the IP. 

::

   sp=bandwidth_1.input:HP0
   sp=bandwidth_1.output:HP0
   sp=bandwidth_2.input:HP1
   sp=bandwidth_2.output:HP1
   sp=bandwidth_3.input:HP2
   sp=bandwidth_3.output:HP2
   sp=bandwidth_4.input:HP3
   sp=bandwidth_4.output:HP3

Following is the real log reported while running the design on zcu102_base platform:

:: 

   INFO: Starting the Verify test....
   INFO: Found Platform
   INFO: Platform Name: Xilinx
   INFO: Platform Version: OpenCL 1.0
   INFO: Platform Profile: EMBEDDED_PROFILE
   INFO: Platform Extensions: cl_khr_icd
   INFO: INFO: Reading validate_zcu.xclbin
   INFO: Loading: 'validate_zcu.xclbin'
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
