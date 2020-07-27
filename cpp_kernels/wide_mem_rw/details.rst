Wide Memory Read/Write (C)
==========================

This example demonstrates the use of ``ap_uint<>`` datatype which
enables ``V++`` to figure out the memory datawidth for the kernel to
global memory transfers.

For efficient use of this bandwidth, ``ap_uint<512>`` is used as
interface between kernels and global memory allows for maximum of 512
bits to be transferred concurrently. User must include ``ap_int.h`` to
use these datatypes.

.. code:: cpp

   #define DATAWIDTH 512
   typedef ap_uint<DATAWIDTH> uint512_dt;

   uint512_dt v1_local[BUFFER_SIZE];
   uint512_dt result_local[BUFFER_SIZE];

Using 512 bits user can enable the ``burst`` read and write for the data
by using ``#pragma HLS PIPELINE``. Auto-pipeline is going to apply
pipeline to these loops.

.. code:: cpp

   v1_rd:
          for (int j = 0; j < chunk_size; j++) {
             #pragma HLS LOOP_TRIPCOUNT min=c_chunk_sz max=c_chunk_sz
              v1_local[j] = in1[i + j];
          }
