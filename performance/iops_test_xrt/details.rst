IOPS Test XRT (XRT Native API's)
================================

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

