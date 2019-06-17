Host Global Bandwidth Test
===========================

This example tests the data transfer bandwidth between host and global memory for 3 different cases : 
    1) host to global memory transfer, 
    2) global memory to host transfer 
    3) and bidirectional transfer.

Different combinations of buffer sizes and number of buffers are created and data to be transferred. For data transfer, `clenqueueMigrateMemObjects()` API is used to transfer the data between host to global memory as this API does immediate data transfer and right choice for bandwidth measurement.
```c++
err = commands.enqueueMigrateMemObjects(mems1, 0/* 0 means from host*/);
err = commands.enqueueMigrateMemObjects(mems2, CL_MIGRATE_MEM_OBJECT_HOST);
```
