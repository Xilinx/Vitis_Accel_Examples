Clock Frequency ~ Too Many Compute Units (C) 
======================

This is simple example of vector addition to demonstrate effectiveness of using single compute unit with heavy work load to achieve better performance. Bad example uses multiple compute units to achieve good performance but it results in heavy usage of FPGA resources and area due to which design fails timing. Good example uses single compute unit to compute with heavier work load, it helps in less resource utilization and also helps in kernel scalability. To switch between Good/Bad cases use the flag provided in makefile.

***KEY CONCEPTS:*** Clock Frequency, Data Level Parallelism, Multiple Compute Units

***KEYWORDS:*** #pragma HLS PIPELINE, #pragma HLS ARRAY_PARTITION


For Setup instructions, please refer: [setup.md][]

[setup.md]: setup.md

