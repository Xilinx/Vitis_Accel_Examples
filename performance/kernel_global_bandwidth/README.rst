Kernel Global Bandwidth
=======================

Bandwidth test of global to local memory.

.. raw:: html

 <details>

.. raw:: html

 <summary> 

 <b>EXCLUDED PLATFORMS:</b>

.. raw:: html

 </summary>
|
..

 - AWS F2
 - Samsung U.2 SmartSSD
 - Alveo U30
 - Alveo U50
 - Alveo U55
 - Versal VCK5000
 - Alveo U250
 - Versal V70

.. raw:: html

 </details>

.. raw:: html

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/kernel.cpp
   src/kernel_global_bandwidth.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./kernel_global_bandwidth <krnl_kernel_global XCLBIN>

DETAILS
-------

This example tests global memory access bandwidth of kernel. Kernel
increase the bandwidth by accessing multiple DDR banks through different
interfaces.

Banks need to be specified using ``sp`` tags in
respective ``.cfg`` file along with the port name and kernel name. For Ex-

::

   [connectivity]
   sp=bandwidth_1.m_axi_gmem0:DDR[0]
   sp=bandwidth_1.m_axi_gmem1:DDR[1]

Following is the log reported while running the design on U200 platform:

::

   ./kernel_global ./build_dir.hw.xilinx_u200_xdma_201830_2/krnl_kernel_global.xclbin
   Found Platform
   Platform Name: Xilinx
   Found Device=xilinx_u200_xdma_201830_2
   INFO: Reading ./build_dir.hw.xilinx_u200_xdma_201830_2/krnl_kernel_global.xclbin
   Loading: './build_dir.hw.xilinx_u200_xdma_201830_2/krnl_kernel_global.xclbin'
   Starting kernel to read/write 1024 MB bytes from/to global memory... 
   Kernel Duration...66662680 ns
   Kernel completed read/write 1024 MB bytes from/to global memory.
   Execution time = 0.066663 (sec) 
   Concurrent Read and Write Throughput = 30.72 (GB/sec) 
   TEST PASSED

GUI Flow :

By default this example supports 1DDR execution in GUI mode for all the
XSAs. To make this example to work with multi DDR XSAs please follow
steps mentioned below. XSA (2DDR):

1. Add a .cfg file in the /src folder with the following entries:
   [Connecttivity] sp=bandwidth_1.m_axi_gmem0:DDR[0]
   sp=bandwidth_1.m_axi_gmem1:DDR[1]
   2. > Properties > C/C++ Build > Settings > Vitis V++ Kernel Compiler
   > Miscellaneous > Other flags 3.In “Other flags” box enter following

   a. –max_memory_ports all 4. > Properties > C/C++ Build > Settings >
      Vitis V++ Kernel Linker > Miscellaneous > Other flags –config
      ../src/<config_file.cfg>

XSA (3DDR):

1. Add a .cfg file in the /src folder with the following entries:
   [Connecttivity] sp=bandwidth_1.m_axi_gmem0:DDR[0]
   sp=bandwidth_1.m_axi_gmem1:DDR[1]
   sp=bandwidth_1.m_axi_gmem2:DDR[2]
   2. > Properties > C/C++ Build > Settings > Vitis V++ Kernel Compiler
   > Miscellaneous > Other flags 3.In “Other flags” box enter following

   a. –max_memory_ports all 4. > Properties > C/C++ Build > Settings >
      Vitis V++ Kernel Linker > Miscellaneous > Other flags –config
      ../src/<config_file.cfg> 5.Define NDDR_BANKS 3 in kernel “#define
      NDDR_BANKS 3” at the top of kernel.cl
