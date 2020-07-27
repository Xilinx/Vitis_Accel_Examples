Copy Buffer
===========

This example illustrates the use of ``clEnqueueCopyBuffer`` API which is
used to copy the contents of a buffer into another buffer in the global
memory without copying the contents to host and back.

Contents of ``buffer_a`` are copied into ``buffer_b``.

.. code:: cpp

   err = q.enqueueCopyBuffer(buffer_a, buffer_b, 0, 0, size_in_bytes);

Note: Only CDMA enabled platforms will copy the buffer directly using
``clEnqueueCopyBuffer`` API. Please check platform Info to make sure if
platform is CDMA enabled or not.
