IOPS Test
=========

This is simple test design to measure Input/Output Operations per second. In this design, a simple kernel is enqueued many times and measuring overall IOPS.

For measuring the IOPS we do kernel enqueue 1 Million times and capture the time it takes to complete -

.. code:: cpp

   std::chrono::high_resolution_clock::time_point TimeStart =
       std::chrono::high_resolution_clock::now();
   for (int i = 0; i < iter; i++) {
     OCL_CHECK(err, err = q.enqueueTask(iops_test));
   }
   q.finish();
   std::chrono::high_resolution_clock::time_point TimeEnd =
       std::chrono::high_resolution_clock::now();
   double duration =
       std::chrono::duration_cast<std::chrono::microseconds>(TimeEnd - TimeStart)
          .count();

Following is the real log reported while running the design on U200
platform:

::

   Time for 1000000 enqueues: 18.62 sec
   Performance : 53705.7 IOPS
   TEST PASSED

