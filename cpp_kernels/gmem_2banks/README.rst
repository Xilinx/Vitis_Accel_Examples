Global Memory Two Banks (C)
===========================

This example of 2ddr is to demonstrate how to use multiple ddr and create buffers in each DDR.

**KEY CONCEPTS:** `Multiple Banks <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Using-Multiple-DDR-Banks>`__

**KEYWORDS:** `m_axi_auto_max_ports <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/MAXI>`__, `sp <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/connectivity-Options>`__, `connectivity <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/connectivity-Options>`__

**EXCLUDED PLATFORMS:** 

 - All Embedded Zynq Platforms, i.e zc702, zcu102 etc
 - All Versal Platforms, i.e vck190 etc
 - Alveo U30
 - Alveo U50
 - Alveo U55
 - Samsung SmartSSD Computation Storage Drive
 - Samsung U.2 SmartSSD
 - X3 Compute Shell
 - All NoDMA Platforms, i.e u50 nodma etc

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   data/golden.bmp
   src/apply_watermark.cpp
   src/host.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./gmem_2banks -x <apply_watermark XCLBIN> -i $(XF_PROJ_ROOT)/common/data/xilinx_img.bmp -c ./data/golden.bmp

DETAILS
-------

This example demonstrates how buffers can be created in two different
DDR(Global memory) banks.

By default, all the memory interfaces from all the kernels are connected
to a single global memory bank. As a result, only one memory interface
at a time can transfer data to and from the memory bank, limiting the
performance of the kernel. If the FPGA contains only one DDR (or global)
memory bank, this is the only option. However, some FPGA devices contain
multiple DDR memory banks which allows us to customize the connections
among the kernel memory interfaces and the DDR memory bank of a device
by altering the default connection.

This example uses watermark application to demonstrate the utilization
of multiple DDRs for full bandwidth. Input image is placed into
``Bank0`` and kernel will read the input Image and write the output
image to ``Bank1``. Banks need to be specified using ``sp`` tags in
apply_watermark.cfg file along with the port name and kernel name.

::

   [connectivity]
   sp=apply_watermark_1.input:DDR[0]
   sp=apply_watermark_1.output:DDR[1]

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.