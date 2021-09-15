Host Memory Simple XRT (XRT Native API's)
=========================================

This example illustrates the use of ``host_only`` Buffer which resides
in the host memory and directly transferred to/from the kernel

.. code:: cpp

   xrt::bo::flags flags = xrt::bo::flags::host_only;
   auto bo0 = xrt::bo(device, size_in_bytes, flags, krnl.group_id(0));

Using host_only flag we mention that the buffer is host only buffer
and the flag is passed as argument during buffer creation.

