Device Only Buffer XRT (XRT Native API's)
=========================================

This example illustrates the use of ``device_only`` Buffer which is
meant to be used only by the kernel.In this example we create buffer
in global memory which are not mapped to host.

.. code:: cpp

   xrt::bo::flags flags = xrt::bo::flags::device_only;
   auto bo_dev_only = xrt::bo(device, size_in_bytes, flags, krnl_mmult.group_id(0));

Using device_only flag we mention that the buffer is device only buffer
and the flag is passed as argument during buffer creation.

