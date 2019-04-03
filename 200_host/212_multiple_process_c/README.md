Multiple Process (C)
======================

This example will demonstrate how to run multiple processes to utilize multiple kernels simultaneously on an FPGA device. Multiple processes can share access to the same device provided each process uses the same xclbin. Processes share access to all device resources but there is no support for exclusive access to resources by any process.

***KEY CONCEPTS:*** Concurrent execution, Multiple HLS kernels, Multiple Process Support

***KEYWORDS:*** PID, fork, XCL_MULTIPROCESS_MODE, multiprocess


For more information, please refer: [details.md][]

[details.md]: details.md

