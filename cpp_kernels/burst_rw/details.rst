Burst Read/write
================

The usual reason for having a burst mode capability, or using burst
mode, is to increase data throughput. This example demonstrates how
multiple items can be read from global memory to kernel’s local memory
in a single burst. This is done to achieve low memory access latency and
also for efficient use of bandwidth provided by the ``m_axi`` interface.
Similarly, computation results are stored in a buffer and are written to
global memory in a burst. Auto-pipeline is going to apply pipeline to
these loops

The for loops used have the following requirements to implement burst
read/write:

-  Pipeline the loop : Loop pipeline must have II(Initiation interval) =
   1 specfied by the pipeline pragma inside the loop.
-  Aligned memory : Memory addresses for read/write should be
   contiguous.
