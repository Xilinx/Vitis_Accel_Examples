IOPS Test XRT (XRT Native API's)
================================

This is simple test design to measure Input/Output Operations per second. In this design, a simple kernel is enqueued many times and measuring overall IOPS using XRT native api's.

**KEY CONCEPTS:** Input/Output Operations per second

**EXCLUDED PLATFORMS:** 

 - All NoDMA Platforms, i.e u50 nodma etc

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/hello.cpp
   src/host.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./iops_test_xrt -x <hello XCLBIN>

DETAILS
-------

This is simple test design to measure Input/Output Operations per second using xrt native api's.
For measuring the IOPS we run kernel 1 Million times and capture the time it takes to complete -

Following is the real log reported while running the design on U250
platform:

::

   Open the device0
   Load the xclbin ./build_dir.hw.xilinx_u250_gen3x16_xdma_3_1_202020_1/hello.xclbin
   Allocated commands, expect 10000, created 10000
   Commands:      10 iops: 84033.6
   Commands:      50 iops: 127226
   Commands:     100 iops: 270270
   Commands:     200 iops: 282486
   Commands:     500 iops: 295159
   Commands:    1000 iops: 299940
   Commands:    1500 iops: 297442
   Commands:    2000 iops: 304599
   Commands:    3000 iops: 310013
   Commands:    5000 iops: 314505
   Commands:   10000 iops: 316937
   Commands:   50000 iops: 335004
   Commands:  100000 iops: 340296
   Commands:  500000 iops: 357458
   Commands: 1000000 iops: 359184
   TEST PASSED


For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.