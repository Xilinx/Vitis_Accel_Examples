Streaming System Compiler
=========================

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
