Dataflow using HLS Stream
============================

This example explains how `#pragma HLS dataflow` can be used to implement task level parallelism using `HLS Stream` datatype.

Usually data stored in the array is consumed or produced in a sequential manner, a more efficient
communication mechanism is to use streaming data as specified by the `STREAM` pragma, where
FIFOs are used instead of RAMs. Depth of `FIFO` can be specified by `depth` option in the pragma.

```c++
#pragma HLS STREAM variable = inStream depth = 32
#pragma HLS STREAM variable = outStream depth = 32
```

Vector addition in kernel is divided into 3 sub-tasks(read, compute_add and write) which are then performed concurrently using `Dataflow`.

```c++
#pragma HLS dataflow
    read_input(in, inStream, size);
    compute_add(inStream, outStream, inc, size);
    write_result(out, outStream, size);
```    
