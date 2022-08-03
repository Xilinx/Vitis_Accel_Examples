Streaming System Compiler
=========================

This is simple example showcasing Free-running PEs with AXI-streams in system compiler environment.

**KEY CONCEPTS:** `Free Running Kernel <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Free-Running-Kernel>`__

**KEYWORDS:** `vpp::stream <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Stream-connections-using-vpp-stream>`__, `hls::axis <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/AXI4-Stream-Interfaces>`__, `hls::stream <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/HLS-Stream-Library>`__

.. raw:: html

 <details>

.. raw:: html

 <summary> 

 <b>EXCLUDED PLATFORMS:</b>

.. raw:: html

 </summary>
|
..

 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - All Versal Platforms, i.e vck190 etc
 - Alveo U25 SmartNIC
 - AWS VU9P F1
 - All NoDMA Platforms, i.e u50 nodma etc

.. raw:: html

 </details>

.. raw:: html

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/main.cpp
   src/stream_krnl.hpp
   src/stream_krnl_hls.cpp
   src/stream_krnl_top.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./host.exe

DETAILS
-------

This example demonstrates on accelerator containing free-running processing elements with feedback AXI-stream connections. The example also showcases the usage of stream depth.

In System Compiler PE's are referred as Streaming Free Running pipelined elements if the PE once programmed on the device is always executing without start/stop per transaction and PE operates on data asychronously with other PEs in the pipeline. Therefore, all of its interfaces (function arguments) must be streams.

1. The user has to use the macro FREE_RUNNING() on the specific function (incr in this case) to instruct System Compiler that it's streaming free running. The tool will automatically ensure that the HW is generated correctly to make it execute as a free running kernel. 

.. code:: cpp

   FREE_RUNNING(incr);

2. The arguments of incr must be using vpp::stream to ensure correct functionality of such kernel.

::

      void fsk_acc::compute(int* A, int* X, int sz) {
      static vpp::stream<AXI, DEPTH> AS;
      static vpp::stream<AXI, DEPTH> XS;

      ldst(A, X, sz, AS, XS);
      incr(AS, XS);}

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.