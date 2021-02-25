IOPS Test
=========

This is simple test design to measure Input/Output Operations per second. In this design, a simple kernel is enqueued many times and measuring overall IOPS.

For measuring the IOPS we do kernel enqueue 1 Million times and capture the time it takes to complete -

.. code:: cpp

   std::chrono::high_resolution_clock::time_point TimeStart =
       std::chrono::high_resolution_clock::now();
   for (unsigned int i = 0; i < iter; i++) {
     OCL_CHECK(err, err = q.enqueueTask(hello));
   }
   q.finish();
   std::chrono::high_resolution_clock::time_point TimeEnd =
       std::chrono::high_resolution_clock::now();
   double duration =
       std::chrono::duration_cast<std::chrono::microseconds>(TimeEnd - TimeStart)
          .count();

Following is the real log reported while running the design on U250
platform:

::

   Loading: './build_dir.hw.xilinx_u250_gen3x16_xdma_3_1_202020_1/hello.xclbin'
   Trying to program device[0]: xilinx_u250_gen3x16_xdma_shell_3_1
   Device[0]: program successful!
   Commands:      10 iops: 42553.2
   Commands:      50 iops: 62972.3
   Commands:     100 iops: 66093.9
   Commands:     200 iops: 65104.2
   Commands:     500 iops: 65634
   Commands:    1000 iops: 68832.6
   Commands:    1500 iops: 70297.1
   Commands:    2000 iops: 71495
   Commands:    3000 iops: 72982
   Commands:    5000 iops: 75482
   Commands:   10000 iops: 75937.6
   Commands:   50000 iops: 86605
   Commands:  100000 iops: 90265.6
   Commands:  500000 iops: 89432
   Commands: 1000000 iops: 89306.1
   TEST PASSED

