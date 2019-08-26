Dataflow Function OpenCL
=========================

This example demonstrates the use of `xcl_dataflow` attribute used to implement task level parallelism in OpenCL kernels i.e. 
multiple functions can be pipelied to increase the throughput of the design.

Kernel `adder` uses 3 functions `read_input` to read inputs from global memory, `compute_add` for addition of these inputs and
`write_output` to write the results back to the global memory.

```c++
__attribute__ ((xcl_dataflow))
void adder(__global int *in, __global int *out, int inc, int size)
{
    int buffer_in[BUFFER_SIZE];
    int buffer_out[BUFFER_SIZE];

    read_input(in,buffer_in,size);
    compute_add(buffer_in,buffer_out,inc,size);
    write_result(out,buffer_out,size);
}
```
