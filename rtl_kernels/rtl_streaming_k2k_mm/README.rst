RTL Stream Kernel to Kernel and Memory Mapped (RTL Kernel)
==========================================================

This example uses two simple increment RTL kernels connected to each other via stream interface and connected to host via memory mapped C++ data mover kernels

**KEY CONCEPTS:** `Read/Write Stream <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Host-Coding-for-Free-Running-Kernels>`__, `RTL Kernel <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/RTL-Kernels>`__

**KEYWORDS:** `ap_axiu <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Streaming-Data-Transfers>`__, `hls::stream <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/HLS-Stream-Library>`__

**EXCLUDED PLATFORMS:** 

 - All Versal Platforms, i.e vck190 etc
 - ZC706 Evaluation board
 - All NoDMA Platforms, i.e u50 nodma etc

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/gen_xo_myadder1.tcl
   src/gen_xo_myadder2.tcl
   src/host.cpp
   src/krnl_mm2s.cpp
   src/krnl_s2mm.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./rtl_streaming_k2k_mm <myadder XCLBIN>

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.