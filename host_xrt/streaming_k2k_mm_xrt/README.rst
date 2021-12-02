Stream Kernel to Kernel Memory Mapped XRT (XRT Native API's)
============================================================

This is a simple kernel to kernel streaming Vector Add and Vector Multiply C Kernel design with 2 memory mapped input to kernel 1, 1 Stream output from kernel 1 to input of kernel 2, 1 memory mapped input to kernel 2, and 1 memory mapped output that demonstrates on how to process a stream of data for computation between two kernels using XRT Native APIs. This design also illustrates how to set FIFO depth for AXIS connections i.e. for the stream connecting the two kernels

**KEY CONCEPTS:** `Read/Write Stream <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Host-Coding-for-Free-Running-Kernels>`__, `Create/Release Stream <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Host-Coding-for-Free-Running-Kernels>`__, `AXIS FIFO depth <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/Specifying-Compiler-Created-FIFO-Depth>`__

**KEYWORDS:** `stream_connect <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Specifying-Streaming-Connections-between-Compute-Units>`__

**EXCLUDED PLATFORMS:** 

 - Samsung SmartSSD Computation Storage Drive
 - Samsung U.2 SmartSSD
 - All Versal Platforms, i.e vck190 etc
 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - All NoDMA Platforms, i.e u50 nodma etc

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/krnl_stream_vadd.cpp
   src/krnl_stream_vmult.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./streaming_k2k_mm_xrt -x <krnl_stream_vadd_vmult XCLBIN>

DETAILS
-------

This example demonstrates how kernels can have memory mapped inputs
along with stream interface from one kernel to another.

``kernel_stream_vadd`` has two memory mapped inputs and one stream
output. ``kernel_stream_vmult`` has one memory mapped input along with
kernel to kernel stream from ``kernel_stream_vadd`` which acts as its
second input.

Kernel arguments are not required to be setup in host code for kernel to
kernel streaming interfaces. Argument 2 for ``kernel_vadd`` and argument
1 for ``kernel_vmult`` are not declared.

.. code:: cpp

   run.set_arg(0,bo0);
   run.set_arg(1,bo1);
   run.set_arg(3,DATA_SIZE);

   run1.set_arg(0,bo2);
   run1.set_arg(2,bo_out);
   run1.set_arg(3,DATA_SIZE);

Producer kernel stream output port must be connected to consumer kernel
stream input port during the ``v++`` linking stage.

::

   --config krnl_stream_vadd_vmult.cfg 

Content of ``krnl_stream_vadd_vmult.cfg`` file is below:

::

   [connectivity]
   stream_connect=krnl_stream_vadd_1.out:krnl_stream_vmult_1.in2:64 

Above specify that ``out`` port of ``krnl_stream_vadd_1`` kernel is
connected to ``in2`` port of ``krnl_stream_vmult_1``. The FIFO depth of this stream is 64.

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.