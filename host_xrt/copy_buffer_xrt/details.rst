Copy Buffer XRT (XRT Native API's)
===========

This example illustrates the use of ``Copyr`` API for deep copy between 
the two buffer objects if the platform supports a deep-copy. If deep copy
is not supported by the platform the data transfer happens by shallow copy.

Contents of ``bo_a`` are copied into ``bo_b``.

.. code:: cpp

   dst_buffer.copy(src_buffer, copy_size_in_bytes);

The API xrt::bo::copy also has overloaded version to provide a different
offset than 0 for both the source and the destination buffer.

