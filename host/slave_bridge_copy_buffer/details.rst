Slave Bridge Copy Buffer(C)
===========================

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

