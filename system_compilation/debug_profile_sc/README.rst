Debug and Profile System Compiler
=================================

This simple example implements a HW pipeline of three tasks, matrix multiplication along with two different increments to the matrix values.

**KEY CONCEPTS:** `Debug <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Enabling-Profile-and-Debug-when-Linking>`__, `Profiling <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Profiling-Optimizing-and-Debugging-the-Application>`__

**KEYWORDS:** `PROFILE_KERNEL <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Debugging-and-Validation>`__, `PROFILE_PORT <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Debugging-and-Validation>`__, `PROFILE_OFFLOAD <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Debugging-and-Validation>`__, `vpp::stream<> <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Stream-connections-using-vpp-stream>`__

.. raw:: html

 <details>

.. raw:: html

 <summary> 

 <b>EXCLUDED PLATFORMS:</b>

.. raw:: html

 </summary>
|
..

 - All NoDMA Platforms, i.e u50 nodma etc
 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - All Versal Platforms, i.e vck190 etc
 - Alveo U55
 - Alveo U25 SmartNIC
 - Samsung U.2 SmartSSD
 - AWS VU9P F1

.. raw:: html

 </details>

.. raw:: html

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/acc.hpp
   src/acc_hls.cpp
   src/acc_wrap.cpp
   src/host.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./host.exe

DETAILS
-------

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

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.