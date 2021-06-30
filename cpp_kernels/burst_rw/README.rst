Burst Read/Write (C)
====================

This is simple example of using AXI4-master interface for burst read and write

**KEY CONCEPTS:** `burst access <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitis_hls_optimization_techniques.html#ddw1586913493144>`__

**KEYWORDS:** `memcpy <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/managing_interface_synthesis.html#qoa1585574520885>`__, `max_read_burst_length <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitis_hls_optimization_techniques.html#mcz1586914389391>`__, `max_write_burst_length <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitis_hls_optimization_techniques.html#mcz1586914389391>`__

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   nodma

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/vadd.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./burst_rw <vadd XCLBIN>

DETAILS
-------

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

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.