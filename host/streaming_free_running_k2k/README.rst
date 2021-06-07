Stream Free Running Kernel (HLS C/C++)
======================================

This is simple example which demonstrate how to use and configure a free running kernel.

**KEY CONCEPTS:** `Free Running Kernel <https://www.xilinx.com/html_docs/xilinx2020_2/vitis_doc/streamingconnections.html#uug1556136182736>`__

**KEYWORDS:** `ap_ctrl_none <https://www.xilinx.com/html_docs/xilinx2020_2/vitis_doc/managing_interface_synthesis.html#qls1539734256651__ae476284>`__, `stream_connect <https://www.xilinx.com/html_docs/xilinx2020_2/vitis_doc/buildingdevicebinary.html#yha1565541199876>`__

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   2018
   qep
   aws-vu9p-f1
   samsung
   zc702
   nodma

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

   ./streaming_free_running_k2k <krnl_incr XCLBIN>

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

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.