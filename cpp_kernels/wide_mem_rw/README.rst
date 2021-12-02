Wide Memory Read/Write (C)
==========================

This is simple example of vector addition to demonstrate Wide Memory Access using ap_uint<512> data type. Based on input argument type, V++ compiler will figure our the memory datawidth between Global Memory and Kernel. For this example, ap_uint<512> datatype is used, so Memory datawidth will be 16 x (integer bit size) = 16 x 32 = 512 bit.

**KEY CONCEPTS:** Kernel to DDR, `wide memory access <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/AXI-Burst-Transfers>`__, `burst read and write <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/AXI-Burst-Transfers>`__

**KEYWORDS:** `ap_uint<> <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/C-Arbitrary-Precision-Integer-Types>`__, `ap_int.h <https://docs.xilinx.com/r/en-US/ug1399-vitis-hls/C-Arbitrary-Precision-Integer-Types>`__

**EXCLUDED PLATFORMS:** 

 - Embedded ZC702
 - All NoDMA Platforms, i.e u50 nodma etc

DESIGN FILES
------------

Application code is located in the src directory. Accelerator binary files will be compiled to the xclbin directory. The xclbin directory is required by the Makefile and its contents will be filled during compilation. A listing of all the files in this example is shown below

::

   src/host.cpp
   src/vadd.cpp
   
COMMAND LINE ARGUMENTS
----------------------

Once the environment has been configured, the application can be executed by

::

   ./wide_mem_rw <vadd XCLBIN>

DETAILS
-------

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

For more comprehensive documentation, `click here <http://xilinx.github.io/Vitis_Accel_Examples>`__.