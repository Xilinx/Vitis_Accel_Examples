Large application Splits into Multiple Kernels(CL)
======================

This is a multi-filter image processing application to showcase effectiveness of Dataflow/Streams usage. This examples is intended to help developers to break down the complex kernel into multiple sub-functions using OpenCL Dataflow. It presents a way to concurrently execute multiple functions with better area utilization compared to a complex single kernel implementation. The main objective of this example is to showcase a way to build a optimal FPGA design which achieves maximum frequency with optimal resource utilization and achieves better performance compared to single kernel implementations.

***KEY CONCEPTS:*** Dataflow, Stream

***KEYWORDS:*** xcl_dataflow, xcl_array_partition, xcl_pipeline_loop


For more information, please refer: [details.md][]

[details.md]: details.md

