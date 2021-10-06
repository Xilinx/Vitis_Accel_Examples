Validate Embedded Platform (C)
==============================

This is a simple design that verifies if the platform has basic functionalities. It also tests the possible bandwidth between Kernel and Global Memory.

**KEY CONCEPTS:** Bandwidth

**EXCLUDED PLATFORMS:** 

 - All Xilinx DMA Platforms
 - Alveo U30
 - AWS VU9P F1
 - Samsung SmartSSD Computation Storage Drive
 - Samsung U.2 SmartSSD
 - All Platforms with 2019 Version
 - All Platforms with 2018 Version
 - Versal VCK5000

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/bandwidth.cpp
   src/host.cpp
   src/verify.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./validate.exe -x <validate XCLBIN>

DETAILS
-------

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

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.