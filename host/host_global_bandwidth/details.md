Host Global Bandwidth
===========================

This example tests the data transfer bandwidth between host and global memory for 3 cases : host to global memory transfer, global memory to host transfer and bidirectional transfer.
For PCIe platform case, Host and device are connected via PCIe interconnect and latency of this interconnect can be seen as the acceleration tax for the application. Hence, these transfers should be efficiently managed.

For bandwidth test, `clenqueueMigrateMemObjects()` API is used to transfer the data from host to global memory and vice versa.
```c++
err = commands.enqueueMigrateMemObjects(mems1, 0/* 0 means from host*/);
err = commands.enqueueMigrateMemObjects(mems2, CL_MIGRATE_MEM_OBJECT_HOST);
```
