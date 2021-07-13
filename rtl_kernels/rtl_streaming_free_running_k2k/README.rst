Stream Free Running Kernel (RTL Kernel)
=======================================

This is simple example which demonstrate how to use and configure a free running RTL kernel.

**KEY CONCEPTS:** `Read/Write Stream <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/streamingconnections.html#ynb1556233012018>`__, `RTL Kernel <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/devrtlkernel.html>`__, `Free Running Kernel <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/streamingconnections.html#uug1556136182736>`__

**KEYWORDS:** `ap_axiu <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/streamingconnections.html#tzq1555344621950>`__, `hls::stream <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/hls_stream_library.html>`__

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   2018
   vck
   zc706
   nodma

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/gen_xo.tcl
   src/host.cpp
   src/krnl_mm2s.cpp
   src/krnl_s2mm.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./rtl_streaming_free_running_k2k <myadder XCLBIN>

DETAILS
-------

This example demonstrates the use of RTL ``free running`` kernel. Free-running kernels have no control signal ports, and cannot be started or stopped. The no-control signal feature of the free-running kernel results in the following characteristics:

-  The free-running kernel has no memory input or output port, and therefore it interacts with the host or other kernels (other kernels can be regular kernel or another free running kernel) only through streams.
-  When the FPGA is programmed by the binary container (xclbin), the free-running kernel starts running on the FPGA, and therefore it does not need the ``clEnqueueTask`` command from the host code.
-  The kernel works on the stream data as soon as it starts receiving from the host or other kernels, and it stalls when the data is not available.
-  The free-running kernel needs a special interface pragma ``ap_ctrl_none`` inside the kernel body.

RTL kernels can be integrated to Vitis using ``RTL Kernel Wizard``. These kernels have the same software interface model as OpenCL and C/C++ kernels. That is, they are seen by the host application as functions with a void return value, scalar arguments and pointer arguments. For free running kernels, Option ``ap_ctrl_none`` has to be chosen while setting up the kernel in the wizard.

The RTL Kernel Wizard automates some of the steps that need to be taken to ensure that the RTL IP is packaged into a kernel that can be integrated into a system in Vitis environment.

For kernel to kernel streaming transfer, kernel to kernel data transfer ports do not need ``clSetKernelArg`` from host code.

Producer kernel ``mem_read`` streaming output port needs to be connected to consumer kernel ``myadder`` stream input port during ``v++`` using ``myadder.cfg`` file as below:

::

   [connectivity]
   stream_connect=krnl_mm2s_1.out:myadder_1.in 
   stream_connect=myadder_1.out:krnl_s2mm_1.in 

This stream connection specifies that ``stream`` streaming port of ``mem_read`` will be connected to ``in`` streaming port of ``myadder`` during system linker run and same for the second stream connection. This connectivity file is provided to v++ command as below:

::

    --config myadder.cfg

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.