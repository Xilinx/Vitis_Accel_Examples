Hello World
=============
This example a simple hello world example to explain the Host and Kernel code structure. Here a simple `vadd` kernel is used to explain the same.


Vitis kernel can have one s_axilite interface which will be used by host application to configure the kernel. Here `bundle=control` is defined which is s_axilite interface and associated with all the arguments (in1, in2, out_r and size). control interface must also be associated with `return`.

```c++
void vadd(const unsigned int *in1, 
          const unsigned int *in2, 
          unsigned int *out_r,     
          int size)                 
#pragma HLS INTERFACE s_axilite port = in1 bundle = control
#pragma HLS INTERFACE s_axilite port = in2 bundle = control
#pragma HLS INTERFACE s_axilite port = out_r bundle = control
#pragma HLS INTERFACE s_axilite port = size bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control
```

All the global memory access arguments are associated to m_axi(AXI Master Interface) as below:
```c++
#pragma HLS INTERFACE m_axi port=in1  offset=slave bundle=gmem
#pragma HLS INTERFACE m_axi port=in2  offset=slave bundle=gmem
#pragma HLS INTERFACE m_axi port=out_r  offset=slave bundle=gmem
```

Here all three arguments `in1`, `in2`, `out_r` are associated to bundle `gmem` which means that one AXI master interface named `gmem` will be created in Kernel and all these variables will be accessing global memory through this interface. Multiple interfaces can also be created based on the requirements. For example when multiple memory accessing arguments need access to global memory simultaneously, user can create multiple master interfaces and can connect to different arguments.


Rather than reading individual items for addition, local buffers are created in kernel local memory and multiple items are read in a single burst. This is done to achieve low memory access latency and also for efficient use of bandwidth provided by the m_axi interface. 

Similarly, results are stored in a buffer and are written to global memory in a burst.
The for loops used have the following requirements to implement burst read/write:

- Pipeline the loop : Loop pipeline must have `II` (Initiation interval) = 1

- contiguous memory : Memory addresses for read/write should be contiguous.

```c++
    read1: for (int j = 0 ; j < chunk_size ; j++){
        #pragma HLS PIPELINE II=1
            v1_buffer[j] = in1[i + j];
        }
    write: for (int j = 0 ; j < chunk_size ; j++){
        #pragma HLS PIPELINE II=1
            out_r[i + j] = vout_buffer[j];
        }    
```
