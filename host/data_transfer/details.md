Host Global Bandwidth Test
===========================

This example tests the data transfer bandwidth between host and global memory for 3 different cases: 
   
    1) Host to global memory transfer
    
    2) Global memory to host transfer 
    
    3) Bidirectional transfer.

Different combinations of buffer sizes and number of buffers are created. For data transfer, `clenqueueMigrateMemObjects()` API is used to transfer the data between host to global memory as this API does immediate data transfer and is the right choice for bandwidth measurement.
```c++
err = commands.enqueueMigrateMemObjects(mems1, 0/* 0 means from host*/);
err = commands.enqueueMigrateMemObjects(mems2, CL_MIGRATE_MEM_OBJECT_HOST);
```
