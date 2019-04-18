Large application Splits into Multiple Kernels (C)
======================

This is a multi-filter image processing application to showcase effectiveness of Dataflow/Streams usage. This examples is intended to help developers to break down the complex kernels into multiple sub-functions using HLS Dataflow/Streams. It presents a way to concurrently execute multiple functions with better area utilization compared to a complex single kernel implementation. The main objective of this example is to showcase a way to build a optimal FPGA design which achieves maximum frequency with optimal resource utilization and achieves better performance compared to single complex kernel implementations.

***KEY CONCEPTS:*** Dataflow, Stream

***KEYWORDS:*** #pragma HLS DATAFLOW, hls::stream, #pragma HLS INLINE, #pragma HLS ARRAY_PARTITION, #pragma HLS PIPELINE


For Setup instructions, please refer: [setup.md][]

[setup.md]: setup.md

