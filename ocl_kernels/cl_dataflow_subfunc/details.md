Dataflow Subfunction OpenCL
============================

This example demonstrates how `xcl_dataflow` attribute can be used to implement task level parallelism for subfunctions inside a function.

`adder` kernel uses a function `run_subfunc`  which has 3 subfunctions `read_input`, `compute_add` and `write_result`.
`xcl_dataflow` is used here to parallelize these subfunctions inside `run_subfunc`.

```c++
__attribute__ ((xcl_dataflow))
void run_subfunc(__global int *in, __global int *out, int inc, int size)
{
    int buffer_in[BUFFER_SIZE];
    int buffer_out[BUFFER_SIZE];

    read_input(in,buffer_in,size);
    compute_add(buffer_in,buffer_out,inc,size);
    write_result(out,buffer_out,size);
}
```
