Device Only Buffer
===================

Device only buffers are used by the kernels to read and write data directly from/to the Device Memory. When xrt allocates memory on the device for a buffer object, it tries to determine which bank to allocate the buffer. Bank assignment can be done explicitly in the host application through the use of cl_mem_ext_ptr or can be automatically inferred based on device CU connectivity.

Host code Guidelines
--------------------------

XRT OpenCL introduces a new buffer extension Flag ``CL_MEM_HOST_NO_ACCESS`` that should be used to denote a Device-only buffer

.. code-block:: c++

    OCL_CHECK(err, cl::Buffer dev_only_buf(context, CL_MEM_READ_WRITE |
                                                      CL_MEM_HOST_NO_ACCESS,
                                         size_in_bytes, nullptr, &err));


