Stream Free Running Kernel (XRT Native API's)
=============================================

This is simple example which demonstrate how to use and configure a free running kernel.

**KEY CONCEPTS:** `Free Running Kernel <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Free-Running-Kernel>`__

**KEYWORDS:** `ap_ctrl_none <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Block-Level-Control-Protocols>`__, `stream_connect <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Specifying-Streaming-Connections>`__

.. raw:: html

 <details>

.. raw:: html

 <summary> 

 <b>EXCLUDED PLATFORMS:</b>

.. raw:: html

 </summary>
|
..

 - All Platforms with 2018 Version
 - qep
 - AWS VU9P F1
 - Samsung SmartSSD Computation Storage Drive
 - All Versal Platforms, i.e vck190 etc
 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - Samsung U.2 SmartSSD
 - All NoDMA Platforms, i.e u50 nodma etc

.. raw:: html

 </details>

.. raw:: html

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/increment.cpp
   src/mem_read.cpp
   src/mem_write.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./streaming_free_running_k2k_xrt -x <krnl_incr XCLBIN>

DETAILS
-------

Free-running kernels have no control signal ports, and cannot be started
or stopped. The no-control signal feature of the free-running kernel
results in the following characteristics:

-  The free-running kernel has no memory input or output port, and
   therefore it interacts with the host or other kernels (other kernels
   can be regular kernel or another free running kernel) only through
   streams.
-  When the FPGA is programmed by the binary container (xclbin), the
   free-running kernel starts running on the FPGA, and therefore it does
   not need the kernel invocation command from the host code.
-  The kernel works on the stream data as soon as it starts receiving
   from the host or other kernels, and it stalls when the data is not
   available.
-  The free-running kernel needs a special interface pragma
   ``ap_ctrl_none`` inside the kernel body.

The free-running kernel only contains ``hls::stream`` inputs and
outputs. The recommended coding guidelines include:

-  The kernel interface should not have any
   ``#pragma HLS interface s_axilite`` or
   ``#pragma HLS interface m_axi`` (as there should not be any memory or
   control port).
-  The kernel interface must contain this special pragma:

.. code:: cpp

   #pragma HLS interface ap_ctrl_none port=return

Producer kernel ``mem_read`` streaming output port needs to be connected
to consumer kernel ``increment`` stream input port during ``v++`` using
``krnl_incr.cfg`` file as below:

::

   [connectivity]
   stream_connect=mem_read_1.stream:increment_1.input
   stream_connect=increment_1.output:mem_write_1.stream

This stream connection specifies that ``stream`` streaming port of
``mem_read`` will be connected to ``input`` streaming port of
``increment`` during system linker run and same for the second stream
connection. This connectivity file is provided to v++ command as below:

::

    --config krnl_incr.cfg
