Wide Memory Read/Write (OpenCL Kernel)
======================================

This example demonstrates the use of ``ap_uint<>`` datatype which
enables ``V++`` to figure out the memory datawidth for the kernel to
global memory transfers.Using this datatype, larger chunks of data can
be transferred as compared to conventional datatypes such as ``int``.

For efficient use of this bandwidth, ``ap_uint<16>`` is used. User must
include ``ap_int.h`` to use these datatypes. \```c++ #define DATAWIDTH
512 typedef ap_uint uint512_dt;

local uint16 v1_local[LOCAL_MEM_SIZE]; local uint16
result_local[LOCAL_MEM_SIZE];

::

   A maximum of 512 bits can be transferred b/w kernel and global memory, attribute `xcl_pipeline_loop` is used to infer the burst read/write.
   ```c++
    v1_rd: __attribute__((xcl_pipeline_loop(1)))
           __attribute__((xcl_loop_tripcount(c_size, c_size)))
           for (int j = 0 ; j <  chunk_size; j++){
               v1_local[j] = in1 [i + j];
           }
