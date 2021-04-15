IOPS Fast Adapter (XRT Native API's)
====================================

This is simple test design to measure Input/Output Operations per second using Fast Adapter. In this design, a simple kernel is enqueued many times and overall IOPS is measured using XRT native api's.

**KEY CONCEPTS:** Input/Output Operations per second, Fast Adapter

**KEYWORDS:** nextDescriptorAddr

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   nodma
   zc
   vck
   u200
   u250

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

   ./iops_fast_adapter_xrt -x <hello XCLBIN>

DETAILS
-------

This is simple design to measure Input/Output Operations per second using Fast Adapter.
For measuring the IOPS we run kernel 1 Million times and capture the time it takes to complete -

We need to make sure Kernel Domain Scheduler(KDS) is enabled. Following are the steps to enable KDS :

.. code:: cpp

       sudo rmmod xocl
       sudo modprobe xocl kds_mode=1
     
The Fast Adapter desriptors should be employed on PLRAM. The same is handled in fa_hello.cfg

.. code:: cpp
       
       [connectivity]
       sp=FA_hello_1.nextDescriptorAddr:PLRAM[0]

Following is the real log reported while running the design on U50
platform:

::

   Open the device0
   Load the xclbin ./build_dir.hw.xilinx_u50_gen3x16_xdma_4_202020_1/hello.xclbin
   Allocated commands, expect 10000, created 10000
   Commands:      10 iops: 90909.1
   Commands:      50 iops: 284091
   Commands:     100 iops: 377358
   Commands:     200 iops: 395257
   Commands:     500 iops: 432900
   Commands:    1000 iops: 436491
   Commands:    1500 iops: 85484.7
   Commands:    2000 iops: 436681
   Commands:    3000 iops: 441371
   Commands:    5000 iops: 411794
   Commands:   10000 iops: 432994
   Commands:   50000 iops: 439858
   Commands:  100000 iops: 441515
   Commands:  500000 iops: 441574
   Commands: 1000000 iops: 443215

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.