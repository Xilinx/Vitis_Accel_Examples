Burst Read/Write (OpenCL Kernel)
================================

This is simple example of using AXI4-master interface for burst read and write

**KEY CONCEPTS:** `burst access <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/AXI-Burst-Transfers>`__

**KEYWORDS:** `compiler.interfaceRdBurstLen <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/advanced-Options>`__, `compiler.interfaceWrBurstLen <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/advanced-Options>`__

**EXCLUDED PLATFORMS:** 

 - All NoDMA Platforms, i.e u50 nodma etc

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/vadd.cl
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./cl_burst_rw <vadd XCLBIN>

DETAILS
-------

The usual reason for having a burst mode capability, or using burst
mode, is to increase data throughput. This example demonstrates how
multiple items can be read from global memory to kernel’s local memory
in a single burst. This is done to achieve low memory access latency and
also for efficient use of bandwidth provided by the ``AXI`` interface.
Similarly, computation results are stored in a buffer and are written to
global memory in a burst.

.. code:: cpp

   __attribute__((xcl_pipeline_loop(1)))
   __attribute__((xcl_loop_tripcount(c_size, c_size)))
           read_buf: for (int j = 0; j < chunk_size; j++) {
               burstbuffer[j] = a[i+j]

The for loops used have the following requirements to implement burst
read/write:

-  Pipeline the loop : Loop pipeline must have II(Initiation interval) =
   1 specfied by the pipeline pragma inside the loop.
-  Aligned memory : Memory addresses for read/write should be
   contiguous.

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.