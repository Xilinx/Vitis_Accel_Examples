Stream Free Running Kernel(HLS C/C++)
=====================================

Free-running kernels have no control signal ports, and cannot be started
or stopped. The no-control signal feature of the free-running kernel
results in the following characteristics:

-  The free-running kernel has no memory input or output port, and
   therefore it interacts with the host or other kernels (other kernels
   can be regular kernel or another free running kernel) only through
   streams.
-  When the FPGA is programmed by the binary container (xclbin), the
   free-running kernel starts running on the FPGA, and therefore it does
   not need the ``clEnqueueTask`` command from the host code.
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

For kernel to kernel streaming transfer, kernel to kernel data transfer
ports do not need ``clSetKernelArg`` from host code.

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
