Slave Bridge Copy Buffer XRT (XRT Native API's)
===============================================

This example illustrates how host only memory can be copied to device only memory and vice-versa using native xrt ``copy`` API.
 
Host only buffer is created using ``host_only`` flag whereas device only buffer is created using ``device_only`` flag. The following data transfers take place in the example -

1. Copy the host_only input buffer ``hostonly_bo`` into  DEVICE_ONLY input buffer ``deviceonly_bo`` using M2M.

.. code:: cpp

  deviceonly_bo0.copy(hostonly_bo0);
  deviceonly_bo1.copy(hostonly_bo1);

2. Enqueue kernel to do computations on DEVICE_ONLY input buffer ``deviceonly_bo`` and put output in device_only output buffer ``deviceonly_bo_out``.

.. code:: cpp

  auto run = krnl(deviceonly_bo0, deviceonly_bo1, deviceonly_bo_out, DATA_SIZE);
  
3. Copy the device_only output buffer ``deviceonly_bo_out`` into  host_only output buffer ``hostonly_bo_out`` using M2M.

.. code:: cpp

  hostonly_bo_out.copy(deviceonly_bo_out);
