Host Global Bandwidth Test for 5.0 Shell
=========================================

This example tests the bandwidth of the interconnect between host and global memory for 3 cases: 
* host to global memory transfer, 
* global memory to host transfer, and 
* bidirectional transfer

Host and device are connected via PCIe interconnect and latency of this interconnect can be seen as the acceleration tax for the application. Hence, these transfers should be efficiently managed.

Different combinations of buffer sizes and number of buffers are created and data to be transferred is filled into buffers using `clEnqueueFillBuffer` API in which pattern with which buffers are to be filled can be specified.
```c++
err = command_queue.enqueueFillBuffer<int>((cl::Buffer&)mems[i], i, 0, nxtcnt, 0, 0);
```

For bandwidth test, `clenqueueMigrateMemObjects()` API is used to transfer the data from host to global memory and vice versa.
```c++
err = commands.enqueueMigrateMemObjects(mems1, 0/* 0 means from host*/);
err = commands.enqueueMigrateMemObjects(mems2, CL_MIGRATE_MEM_OBJECT_HOST);
```

__Note :__ From software emulation log it can be seen that for larger number of buffers of small sizes , throughput achieved for transfer is less whereas for small number of buffers with larger sizes, throughput achieved for transfer is more.
