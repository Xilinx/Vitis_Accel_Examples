Host Memory Simple
==================

Host Memory IP is used by the kernels to read and write data directly from/to the Host Memory. Unlike the XDMA data transfer, this data transfer mechanism does not utlize global memories (DDR, HBM, PLRAM ,etc) on the card. Host-memory provides DMA bypass capability that is primarily used for data transfer on a No-DMA platform.

Kernel Compilation
------------------

Use the following V++ configuration option to connect a kernel AXI-Master Port to Host-memory IP.

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
