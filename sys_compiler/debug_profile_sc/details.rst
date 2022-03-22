Debug and Profile System Compiler
=================================

This example demonstrates the debug and profile options in system compiler. 
When run with TARGET=hw_emu, it can be used to see the results of HW emulation waveforms automatically created.
When run with TARGET=hw, and it can be used to see the timeline traces (auto created) from a profiling run on the card. By default host-side traces are created, and device traces are enabled with configurations in include/<platform>/profile.hpp and using -D ENABLE_HW_PROFILING=1

Input from the user
--------------------
1. Create a file with following profile options and include the same in accelerator: 

.. code:: cpp

   PROFILE_KERNEL("all");
   // example: only specific PEs
   // PROFILE_KERNEL("incr_20[0] incr_20[1]");

   PROFILE_PORT("all");
   // example: only specific ports
   // PROFILE_PORT("incr_20[0]/Y incr_20[1]/E");

   // PROFILE_OFFLOAD("FIFO" | "DDR[0-3]" | "HBM[0-31]" );
   PROFILE_OFFLOAD("FIFO");

2. In the Makefile, add the following flag to enable profiling:

::

      EXTRA_VPPFLAGS += -D ENABLE_HW_PROFILING=1
