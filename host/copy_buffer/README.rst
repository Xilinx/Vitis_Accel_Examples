Copy Buffer (C)
===============

This Copy Buffer example demonstrate how one buffer can be copied from another buffer.

**KEY CONCEPTS:** Copy Buffer

**KEYWORDS:** `cl::CommandQueue <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Command-Queues>`__, enqueueCopyBuffer, `enqueueWriteBuffer <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Buffer-Creation-and-Data-Transfer>`__, `enqueueReadBuffer <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Buffer-Creation-and-Data-Transfer>`__, `enqueueMigrateMemObjects <https://docs.xilinx.com/r/en-US/ug1393-vitis-application-acceleration/Buffer-Creation-and-Data-Transfer>`__

**EXCLUDED PLATFORMS:** 

 - All NoDMA Platforms, i.e u50 nodma etc

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/vector_addition.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./copy_buffer <vector_addition XCLBIN>

DETAILS
-------

This example illustrates the use of ``clEnqueueCopyBuffer`` API which is
used to copy the contents of a buffer into another buffer in the global
memory without copying the contents to host and back.

Contents of ``buffer_a`` are copied into ``buffer_b``.

.. code:: cpp

   err = q.enqueueCopyBuffer(buffer_a, buffer_b, 0, 0, size_in_bytes);

Note: Only CDMA enabled platforms will copy the buffer directly using
``clEnqueueCopyBuffer`` API. Please check platform Info to make sure if
platform is CDMA enabled or not.

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.