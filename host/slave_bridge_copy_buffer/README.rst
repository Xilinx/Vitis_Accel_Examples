Slave Bridge Copy Buffer(C)
===========================

This is simple slave bridge example to describe how host-only memory can be copied to device-only memory and vice-versa.

**KEY CONCEPTS:** slave bridge

**KEYWORDS:** XCL_MEM_EXT_HOST_ONLY, CL_MEM_HOST_NO_ACCESS, enqueueCopyBuffer

EXCLUDED PLATFORMS
------------------

Platforms containing following strings in their names are not supported for this example :

::

   _u25_
   u30
   u50c
   u50lv
   u50_gen3x4
   u55
   u250
   zc
   vck
   aws-vu9p-f1
   2019
   2018
   _u2_

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/krnl_vadd.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./slave_bridge_copy_buffer.exe <krnl_vadd XCLBIN>

DETAILS
-------

This example illustrates how host only memory can be copied to device only memory and vice-versa using ``EnqueueCopyBuffer`` API.
 
Host only buffer is created using ``XCL_MEM_EXT_HOST_ONLY`` flag whereas device only buffer is created using ``CL_MEM_HOST_NO_ACCESS`` flag. The following data transfers take place in the example -

1. Copy the HOST_ONLY input buffer ``buffer_in_host`` into  DEVICE_ONLY input buffer ``buffer_in_dev`` using M2M.

.. code:: cpp

  OCL_CHECK(err,
            err = q.enqueueCopyBuffer(buffer_in_host_a, buffer_in_dev_a, 0, 0,
                                      size_in_bytes, 0, nullptr)); // transfer
  OCL_CHECK(err,
            err = q.enqueueCopyBuffer(buffer_in_host_b, buffer_in_dev_b, 0, 0,
                                      size_in_bytes, 0, nullptr)); // transfer

2. Enqueue kernel to do computations on DEVICE_ONLY input buffer ``buffer_in_dev`` and put output in DEVICE_ONLY output buffer ``buffer_out_dev``.

.. code:: cpp

  q.enqueueTask(krnl);

3. Copy the DEVICE_ONLY output buffer ``buffer_out_dev`` into  HOST_ONLY output buffer ``buffer_out_host`` using M2M.

.. code:: cpp

  OCL_CHECK(err,
            err = q.enqueueCopyBuffer(buffer_out_dev, buffer_out_host, 0, 0,
                                      size_in_bytes, 0, nullptr)); // transfer


For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.