Slave Bridge Simple(C)
======================

This is simple slave bridge example to describe how a user kernel can access the host memory. The host memory allocation is done through the host code. The kernel reads data from host memory and writes result to host memory.

**KEY CONCEPTS:** slave bridge, address translation unit

**KEYWORDS:** XCL_MEM_EXT_HOST_ONLY, HOST[0]

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   _u25_
   u30
   u50lv
   u50_gen3x4
   zc
   vck
   aws-vu9p-f1
   2019
   2018
   samsung
   _u2_

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/kernel.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./slave_bridge_simple.exe <krnl_vadd XCLBIN>

DETAILS
-------

Slave Bridge IP is used by the kernels to read and write data directly from/to the Host Memory. Unlike the XDMA data transfer, this data transfer mechanism does not utlize global memories (DDR, HBM, PLRAM ,etc) on the card. Slave-Bridge provides DMA bypass capability that is primarily used for data transfer on a No-DMA platform.

Kernel Compilation
------------------

Use the following V++ configuration option to connect a kernel AXI-Master Port to Slave-Bridge IP.

.. code-block:: bash

   [connectivity]
   sp=krnl_vadd_1.m_axi_gmem:HOST[0]

Host code Guidelines
--------------------------

XRT OpenCL introduces a new buffer extension Flag ``XCL_MEM_EXT_HOST_ONLY`` that should be used to denote a Host-only buffer

.. code-block:: c++

    cl_mem_ext_ptr_t host_buffer_ext;
    host_buffer_ext.flags = XCL_MEM_EXT_HOST_ONLY;
    host_buffer_ext.obj = nullptr;
    host_buffer_ext.param = 0;

    OCL_CHECK(err, cl::Buffer buffer_in_a(context, CL_MEM_READ_ONLY |
                                                     CL_MEM_EXT_PTR_XILINX,
                                        size_in_bytes, &host_buffer_ext, &err));


For more information about slave bridge feature, please refer following XRT 
`document <https://xilinx.github.io/XRT/master/html/sb.html>`__.


For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.