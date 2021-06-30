Global Memory Two Banks (OpenCL Kernel)
=======================================

This example of 2ddr to demonstrate on how to use 2ddr XSA. How to create buffers in each DDR.

**KEY CONCEPTS:** `Multiple Banks <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/optimizingperformance.html#uuy1504034303412>`__

**KEYWORDS:** `sp <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitiscommandcompiler.html#clt1568640709907__section_tfc_zxm_1jb>`__, `connectivity <https://www.xilinx.com/html_docs/xilinx2021_1/vitis_doc/vitiscommandcompiler.html#qcm1528577331870__section_wgd_dxf_dnb>`__

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   u30
   u50
   u55
   vck
   zc
   samsung
   _u2_
   nodma

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   data/golden.bmp
   src/apply_watermark.cl
   src/host.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./cl_gmem_2banks -x <apply_watermark XCLBIN> -i $(XF_PROJ_ROOT)/common/data/xilinx_img.bmp -c ./data/golden.bmp

DETAILS
-------

This example demonstrates how buffers can be created in two different
DDR(Global memory) banks.

By default, all the memory interfaces from all the kernels are connected
to a single global memory bank. As a result, only one memory interface
at a time can transfer data to and from the memory bank, limiting the
performance of the kernel. If the FPGA contains only one ``DDR`` (or
global) memory bank, this is the only option. However, some FPGA devices
contain multiple DDR memory banks. You can customize the connections
among the kernel memory interfaces and the DDR memory bank of such a
device by altering the default connection.

Banks need to be specified using ``sp`` tags in
apply_watermark.cfg file along with the port name and kernel name.

::

   [connectivity]
   sp=apply_watermark_1.input:DDR[0]
   sp=apply_watermark_1.output:DDR[1]

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.