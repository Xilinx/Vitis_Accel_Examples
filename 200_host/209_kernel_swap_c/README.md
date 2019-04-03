Vector Add - Vector Multiplication for XPR Devices
======================

This example shows how host can swap the kernels and share same buffer between two kernels which are exist in separate binary containers. Dynamic platforms does not persist the buffer data so host has to migrate data from device to host memory before swapping the next kernel. After kernel swap, host has to migrate the buffer back to device.

***KEY CONCEPTS:*** Handling Buffer sharing across multiple binaries, Multiple Kernel Binaries

***KEYWORDS:*** clEnqueueMigrateMemObjects(), CL_MIGRATE_MEM_OBJECT_HOST


For more information, please refer: [details.md][]

[details.md]: details.md

