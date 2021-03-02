Slave Bridge Copy Kernel XRT (XRT Native API's)
===============================================

Here a user copy kernel is instantiated inside container which copies data between host and device memory.
 
Host only buffer is created using ``host_only`` flag whereas device only buffer is created using ``device_only`` flag. The following data transfers take place in the example -

1. Enqueue kernel ``copy_kernel`` to copy the HOST_ONLY input buffer ``hostonly_bo`` into  DEVICE_ONLY input buffer ``deviceonly_bo``.

2. Enqueue kernel ``krnl_vadd`` to do computations on DEVICE_ONLY input buffer ``deviceonly_bo`` and put output in DEVICE_ONLY output buffer ``deviceonly_bo_out``.

3. Enqueue kernel ``copy_kernel`` to copy the DEVICE_ONLY output buffer ``deviceonly_bo_out`` into  HOST_ONLY output buffer ``hostonly_bo_out``.
