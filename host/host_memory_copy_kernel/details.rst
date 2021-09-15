Host Memory Copy Kernel(C)
==========================

Here a user copy kernel is instantiated inside container which copies data between host and device memory.
 
Host only buffer is created using ``XCL_MEM_EXT_HOST_ONLY`` flag whereas device only buffer is created using ``CL_MEM_HOST_NO_ACCESS`` flag. The following data transfers take place in the example -

1. Enqueue kernel ``copy_kernel`` to copy the HOST_ONLY input buffer ``buffer_in_host`` into  DEVICE_ONLY input buffer ``buffer_in_dev``.

2. Enqueue kernel ``krnl_vadd`` to do computations on DEVICE_ONLY input buffer ``buffer_in_dev`` and put output in DEVICE_ONLY output buffer ``buffer_out_dev``.

3. Enqueue kernel ``copy_kernel`` to copy the DEVICE_ONLY output buffer ``buffer_out_dev`` into  HOST_ONLY output buffer ``buffer_out_host``.
